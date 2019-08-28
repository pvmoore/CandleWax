package candlewax

import candlewax.Scene.MaterialArray.Material
import candlewax.Scene.SDFObjectArray.SDFObject
import org.apache.log4j.Logger
import org.joml.*
import org.lwjgl.glfw.GLFW.*
import org.lwjgl.vulkan.VK10.*
import org.lwjgl.vulkan.VkCommandBuffer
import org.lwjgl.vulkan.VkImageMemoryBarrier
import vulkan.api.*
import vulkan.api.descriptor.VkDescriptorSet
import vulkan.api.descriptor.bindDescriptorSets
import vulkan.api.image.VkImage
import vulkan.api.memory.allocImage
import vulkan.api.pipeline.ComputePipeline
import vulkan.api.pipeline.bindPipeline
import vulkan.app.*
import vulkan.common.*
import vulkan.d2.Camera2D
import vulkan.d2.FPS
import vulkan.d2.Quad
import vulkan.d2.Text
import vulkan.d3.Camera3D
import vulkan.maths.copy
import vulkan.misc.RGBA
import vulkan.misc.megabytes
import vulkan.misc.orThrow
import vulkan.misc.toVector3f
import vulkan.texture.Textures

class Scene {
    companion object {
        val log = Logger.getLogger(this::class.java)
    }
    private lateinit var vk:VulkanApplication
    private lateinit var graphics:GraphicsComponent
    private lateinit var context:RenderContext

    private class FrameResource(
        val computeCmd      : VkCommandBuffer,
        val computeFinished : VkSemaphore,
        val computeTarget   : VkImage,
        val descriptorSet   : VkDescriptorSet,
        val quad            : Quad
    )
    private class UBO(
        val view       : Matrix4f = Matrix4f(),
        val invView    : Matrix4f = Matrix4f(),
        val cameraPos  : Vector3f = Vector3f(),
        var tanfov2    : Float    = 0f,
        val cameraDir  : Vector3f = Vector3f(),
        var numObjects : Int      = 0,
        val cameraUp   : Vector3f = Vector3f(),
        val _pad2      : Float    = 0f
    ) : AbsUBO()

    private class SDFObjectArray : AbsStorageBuffer<SDFObject>(1000) {
        class SDFObject(
            val move:Vector3f,
            val type:Int,
            val params1:Vector4f,
            val params2:Vector4f,
            val materialIndex:Int,
            val _pad1:Float = 0f,
            val _pad2:Float = 0f,
            val _pad3:Float = 0f)
            : AbsTransferable()

        init{
            assert(elementInstance().size()%16==0)
        }

        override fun elementInstance() = SDFObject(Vector3f(),0,Vector4f(), Vector4f(),0)

        var numObjects:Int = 0
        var isUploaded     = false

        fun add(obj:SDFObject) {
            val size = elementInstance().size()

            stagingBuffer.rangeOf(numObjects*size, size).mapForWriting { b->
                obj.writeTo(b)
            }

            numObjects++
            setStale()
        }
        fun upload(cmd:VkCommandBuffer) {
            if(!isUploaded) {
                transferRange(cmd, 0, numObjects * elementInstance().size())
                isUploaded = true
            }
        }
    }
    private class MaterialArray : AbsStorageBuffer<MaterialArray.Material>(100) {

        class Material(val rgb:Vector3f,
                       val textureIndex:Int,
                       val specularPower:Int,
                       val _pad1:Float = 0f,
                       val _pad2:Float = 0f,
                       val _pad3:Float = 0f) : AbsTransferable()

        override fun elementInstance() = Material(Vector3f(), 0, 0)
    }

    private val camera2d         = Camera2D()
    private val camera3d         = Camera3D()
    private val memory           = VulkanMemory()
    private val buffers          = VulkanBuffers()
    private val descriptors      = Descriptors()
    private val pipeline         = ComputePipeline()
    private val pushConstants    = PushConstants(1)
    private val textures         = Textures()
    private val clearColour      = ClearColour(RGBA(0f,0f,0f,1f))
    private val fps              = FPS(colour = RGBA(1.0f, 1.0f, 0.2f, 1f))
    private val text             = Text()
    private val ubo              = UBO()
    private val sdfObjects       = SDFObjectArray()
    private val materialsBuffer  = MaterialArray()
    private val frameResources   = ArrayList<FrameResource>()

    private val graphicsToComputeImgBarrier = VkImageMemoryBarrier.calloc(1)
    private val computeToGraphicsImgBarrier = VkImageMemoryBarrier.calloc(1)

    private var compCommandPool  = null as VkCommandPool?
    private var textureSampler   = null as VkSampler?
    private var quadSampler      = null as VkSampler?

    private var dragForward = null as Vector3f?
    private var dragUp      = null as Vector3f?

    fun init(vk:VulkanApplication) {
        this.vk       = vk
        this.graphics = vk.graphics

        initialise()
    }
    fun destroy() {
        vk.device.let {
            it.waitForIdle()

            frameResources.forEach { fr->
                fr.computeFinished.destroy()
                fr.computeTarget.destroy()
                fr.quad.destroy()
            }

            clearColour.destroy()

            textureSampler?.destroy()
            quadSampler?.destroy()
            compCommandPool?.destroy()

            graphicsToComputeImgBarrier.free()
            computeToGraphicsImgBarrier.free()

            sdfObjects.destroy()
            materialsBuffer.destroy()

            text.destroy()
            fps.destroy()
            ubo.destroy()
            descriptors.destroy()
            pipeline.destroy()
            textures.destroy()
            buffers.destroy()
            memory.destroy()
        }
    }
    fun update(frame: FrameInfo, res: PerFrameResource) {
        var cameraMoved = false

        if(graphics.inputState.isKeyDown(GLFW_KEY_LEFT)) {
            camera3d.roll(-1.0f * frame.perSecond.toFloat())
            cameraMoved = true
        } else if(graphics.inputState.isKeyDown(GLFW_KEY_RIGHT)) {
            camera3d.roll(1.0f * frame.perSecond.toFloat())
            cameraMoved = true
        }

        graphics.drainWindowEvents().forEach {
            when(it) {
                is KeyEvent -> {
                    when(it.key) {
                        GLFW_KEY_ESCAPE -> graphics.postCloseMessage()
                    }
                }
                is MouseButtonEvent -> {

                }
                is MouseMoveEvent -> {

                }
                is MouseWheelEvent -> {
                    val zoom = it.yDelta * 500f * frame.perSecond.toFloat()
                    camera3d.moveForward(zoom)
                    cameraMoved = true
                }
                is MouseDragStart -> {
                    dragForward = camera3d.forward.copy()
                    dragUp      = camera3d.up.copy()
                    log.info("dragStart $dragForward")
                }
                is MouseDrag -> {

                    camera3d.setOrientation(dragForward!!, dragUp!!)

                    camera3d.yaw(-it.delta.x * 0.002f)
                    camera3d.pitch(it.delta.y * 0.002f)

                    cameraMoved = true
                }
                is MouseDragEnd -> {

                }
            }
        }
        if(cameraMoved) {
            log.debug("camera moved: $camera3d")
            cameraChanged()
        }
    }
    fun render(frame: FrameInfo, res: PerFrameResource) {

        pushConstants.set(0, frame.seconds.toFloat())

        val fr = frameResources[res.index]

        res.cmd.let { b->
            b.beginOneTimeSubmit()

            sdfObjects.upload(b)
            materialsBuffer.transfer(b)

            text.beforeRenderPass(frame, res)
            fps.beforeRenderPass(frame, res)
            fr.quad.beforeRenderPass(frame, res)

            /**
             * Record and execute compute pass
             */
            recordCompute(fr)

            vk.queues.get(Queues.COMPUTE).submit(
                cmdBuffers       = arrayOf(fr.computeCmd),
                waitSemaphores   = arrayOf(),
                waitStages       = intArrayOf(),
                signalSemaphores = arrayOf(fr.computeFinished),
                fence            = null
            )

            /**
             *  Transform computeTarget image to fragment shader read
             */
            computeToGraphicsImgBarrier.image(fr.computeTarget.handle)

            b.pipelineBarrier(
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,      // dependency flags
                null,   // memory barriers
                null,   // buffer barriers
                computeToGraphicsImgBarrier
            )


            // Renderpass initialLayout = UNDEFINED
            //                   loadOp = CLEAR
            b.beginRenderPass(
                context.renderPass!!,
                res.frameBuffer,
                clearColour.value,
                graphics.swapChain.area,
                true
            )

            fr.quad.insideRenderPass(frame, res)
            fps.insideRenderPass(frame, res)
            text.insideRenderPass(frame, res)

            // Renderpass finalLayout = PRESENT_SRC_KHR
            b.endRenderPass()
            b.end()

            /** Submit render buffer */
            vk.queues.get(Queues.GRAPHICS).submit(
                cmdBuffers       = arrayOf(b),
                waitSemaphores   = arrayOf(fr.computeFinished, res.imageAvailable),
                waitStages       = intArrayOf(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT),
                signalSemaphores = arrayOf(res.renderFinished),
                fence            = res.fence
            )
        }
    }
    //#########################################################################################
    private fun initialise() {
        val windowSize = vk.graphics.windowSize

        assert(windowSize.x%8==0) { "Todo - Handle odd screen sizes" }
        assert(windowSize.y%8==0) { "Todo - Handle odd screen sizes" }

        camera2d.resizeWindow(windowSize)

        camera3d.run {
            init(position = Vector3f(0f, 30f, 150f), focalPoint = Vector3f(0f, 0f, 0f))
            resizeWindow(windowSize)
            fovNearFar(Math.toRadians(45.0).toFloat(), 10f, 100_000f)
            //rollByDegrees(180.0)
            log.info("camera3d = $this")

            cameraChanged()
        }

        memory.init(vk)
            .createOnDevice(VulkanMemory.DEVICE, 256.megabytes())
            .createStagingUpload(VulkanMemory.STAGING_UPLOAD, 32.megabytes())

        buffers.init(vk)
            .createStagingUploadBuffer(
                VulkanBuffers.STAGING_UPLOAD,
                memory.get(VulkanMemory.STAGING_UPLOAD),
                32.megabytes()
            )
            .createStorageBuffer(VulkanBuffers.STORAGE, memory.get(VulkanMemory.DEVICE), 32.megabytes())
            .createVertexBuffer(VulkanBuffers.VERTEX, memory.get(VulkanMemory.DEVICE), 1.megabytes())
            .createVertexBuffer(VulkanBuffers.INDEX, memory.get(VulkanMemory.DEVICE), 1.megabytes())
            .createUniformBuffer(VulkanBuffers.UNIFORM, memory.get(VulkanMemory.DEVICE), 1.megabytes())

        context = RenderContext(vk, vk.device, vk.graphics.renderPass, buffers)

        sdfObjects.init(context)
        materialsBuffer.init(context)

        setBufferData()
        setMaterials()

        ubo.init(context)

        textures.init(vk, 16.megabytes())

        textureSampler = context.device.createSampler { info ->
            info.anisotropyEnable(true)
            info.maxAnisotropy(16f)
            info.addressModeU(VK_SAMPLER_ADDRESS_MODE_REPEAT)
            info.addressModeV(VK_SAMPLER_ADDRESS_MODE_REPEAT)
        }

        quadSampler = context.device.createSampler { info->
            info.addressModeU(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
            info.addressModeV(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
        }

        /**
         * Bindings:
         *    0     storage buffer (objects)
         *    1     storage buffer (materials)
         *    2     image          (compute target)
         *    3     uniform buffer
         *    4     textureSampler
         */
        descriptors
            .init(context)
            .createLayout()
                .storageBuffer(VK_SHADER_STAGE_COMPUTE_BIT)
                .storageBuffer(VK_SHADER_STAGE_COMPUTE_BIT)
                .storageImage(VK_SHADER_STAGE_COMPUTE_BIT)
                .uniformBuffer(VK_SHADER_STAGE_COMPUTE_BIT)
                .combinedImageSampler(VK_SHADER_STAGE_COMPUTE_BIT)
                .numSets(vk.graphics.swapChain.numImages)
            .build()

        pipeline
            .init(context)
            .withDSLayouts(arrayOf(descriptors.layout(0).dsLayout))
            .withShader("render.comp")
            .withPushConstants(firstIndex = 0, count = pushConstants.count)
            .build()

        fps.init(context)
            .camera(camera2d)

        text.init(context, graphics.fonts.get("segoe-ui"), 1000, true)
            .camera(camera2d)
            .setSize(16f)
            .appendText("Use the mouse wheel to zoom in/out. Click and drag to look around. Arrow left/right to roll",
                Vector2f(3f,0f))

        compCommandPool = context.device.createCommandPool(
            vk.queues.getFamily(Queues.COMPUTE).index,
            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT or
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
        )

        createFrameResources(windowSize)

        createImageBarriers()
    }
    private fun setBufferData() {

        sdfObjects.add(SDFObject(
            type            = 0,
            move            = Vector3f(30f,1f,0f),
            materialIndex   = 0,
            params1         = Vector4f(5f,0f,0f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 0,
            move            = Vector3f(-30f,0f,0f),
            materialIndex   = 1,
            params1         = Vector4f(5f,0f,0f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 1,
            move            = Vector3f(30f,10f,0f),
            materialIndex   = 0,
            params1         = Vector4f(5f,5f,5f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 1,
            move            = Vector3f(-30f,10f,0f),
            materialIndex   = 1,
            params1         = Vector4f(5f,5f,5f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 3,
            move            = Vector3f(30f,-10f,0f),
            materialIndex   = 0,
            params1         = Vector4f(5f,5f,5f,2f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 3,
            move            = Vector3f(-30f,-10f,0f),
            materialIndex   = 1,
            params1         = Vector4f(5f,5f,5f,2f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 5,
            move            = Vector3f(30f,20f,0f),
            materialIndex   = 0,
            params1         = Vector4f(5f,2f,0f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 5,
            move            = Vector3f(30f,-20f,0f),
            materialIndex   = 0,
            params1         = Vector4f(5f,2f,0f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 5,
            move            = Vector3f(-30f,20f,0f),
            materialIndex   = 1,
            params1         = Vector4f(5f,2f,0f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 5,
            move            = Vector3f(-30f,-20f,0f),
            materialIndex   = 1,
            params1         = Vector4f(5f,2f,0f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 4,
            move            = Vector3f(40f,0f,0f),
            materialIndex   = 0,
            params1         = Vector4f(5f,2f,0f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 4,
            move            = Vector3f(-40f,0f,0f),
            materialIndex   = 1,
            params1         = Vector4f(5f,2f,0f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 6,
            move            = Vector3f(40f,10f,0f),
            materialIndex   = 0,
            params1         = Vector4f(5f,0.1f,5f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 6,
            move            = Vector3f(40f,-10f,0f),
            materialIndex   = 0,
            params1         = Vector4f(0.1f,5f,5f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 6,
            move            = Vector3f(-40f,10f,0f),
            materialIndex   = 1,
            params1         = Vector4f(5f,0.1f,5f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 6,
            move            = Vector3f(-40f,-10f,0f),
            materialIndex   = 1,
            params1         = Vector4f(0.1f,5f,5f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 7,
            move            = Vector3f(40f,20f,0f),
            materialIndex   = 0,
            params1         = Vector4f(7f,5f,0f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 7,
            move            = Vector3f(-40f,20f,0f),
            materialIndex   = 1,
            params1         = Vector4f(7f,5f,0f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 8,
            move            = Vector3f(40f,-20f,0f),
            materialIndex   = 0,
            params1         = Vector4f(5f,3f,1f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 8,
            move            = Vector3f(-40f,-20f,0f),
            materialIndex   = 1,
            params1         = Vector4f(5f,3f,1f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 9,
            move            = Vector3f(40f,30f,0f),
            materialIndex   = 0,
            params1         = Vector4f(2f,5f,0f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 9,
            move            = Vector3f(-40f,30f,0f),
            materialIndex   = 1,
            params1         = Vector4f(2f,5f,0f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 10,
            move            = Vector3f(40f,-30f,0f),
            materialIndex   = 0,
            params1         = Vector4f(5f,2f,3f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 10,
            move            = Vector3f(-40f,-30f,0f),
            materialIndex   = 1,
            params1         = Vector4f(5f,2f,3f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 11,
            move            = Vector3f(40f,40f,0f),
            materialIndex   = 0,
            params1         = Vector4f(5f,2f,0f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 11,
            move            = Vector3f(-40f,40f,0f),
            materialIndex   = 1,
            params1         = Vector4f(5f,2f,0f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 12,
            move            = Vector3f(40f,-40f,0f),
            materialIndex   = 0,
            params1         = Vector4f(5f,2f,0f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 12,
            move            = Vector3f(-40f,-40f,0f),
            materialIndex   = 1,
            params1         = Vector4f(5f,2f,0f,0f),
            params2         = Vector4f()
        ))

        sdfObjects.add(SDFObject(
            type            = 13,
            move            = Vector3f(40f,50f,0f),
            materialIndex   = 0,
            params1         = Vector4f(5f,0f,0f,0f),
            params2         = Vector4f()
        ))
        sdfObjects.add(SDFObject(
            type            = 13,
            move            = Vector3f(-40f,50f,0f),
            materialIndex   = 1,
            params1         = Vector4f(5f,0f,0f,0f),
            params2         = Vector4f()
        ))

        ubo.numObjects = sdfObjects.numObjects
        ubo.setStale()
    }
    private fun setMaterials() {

        materialsBuffer.stagingBuffer.mapForWriting { b->
            Material((RGBA(1f,0.4f,0f)*0.75f).toVector3f(), -1, 64).writeTo(b) // [0] brown
            Material((RGBA(1f,0.8f,0f)*0.75f).toVector3f(), -1, 64).writeTo(b) // [1] yellow
            Material((RGBA(0f,0.8f,1f)*0.75f).toVector3f(), -1, 64).writeTo(b) // [2] blue
            Material((RGBA(0.8f,1f,0f)*0.75f).toVector3f(), -1, 64).writeTo(b) // [3] green
            Material((RGBA(1f,0f,1f)*0.5f).toVector3f(),    -1, 64).writeTo(b) // [4] purple
            Material((RGBA(1f,0f,0f)*0.5f).toVector3f(),   -1, 64).writeTo(b) // [5] red
        }
    }
    private fun createImageBarriers() {
        graphicsToComputeImgBarrier
            .sType(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER)
            .oldLayout(VK_IMAGE_LAYOUT_UNDEFINED)
            .newLayout(VK_IMAGE_LAYOUT_GENERAL)
            .srcAccessMask(0)
            .dstAccessMask(VK_ACCESS_SHADER_WRITE_BIT)
            .srcQueueFamilyIndex(vk.queues.getFamily(Queues.GRAPHICS).index)
            .dstQueueFamilyIndex(vk.queues.getFamily(Queues.COMPUTE).index)

        graphicsToComputeImgBarrier.subresourceRange()
            .aspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
            .baseMipLevel(0)
            .levelCount(VK_REMAINING_MIP_LEVELS)
            .baseArrayLayer(0)
            .layerCount(VK_REMAINING_ARRAY_LAYERS)


        computeToGraphicsImgBarrier
            .sType(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER)
            .oldLayout(VK_IMAGE_LAYOUT_GENERAL)
            .newLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            .srcAccessMask(VK_ACCESS_SHADER_WRITE_BIT)
            .dstAccessMask(VK_ACCESS_SHADER_READ_BIT)
            .srcQueueFamilyIndex(vk.queues.getFamily(Queues.COMPUTE).index)
            .dstQueueFamilyIndex(vk.queues.getFamily(Queues.GRAPHICS).index)

        computeToGraphicsImgBarrier.subresourceRange()
            .aspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
            .baseMipLevel(0)
            .levelCount(VK_REMAINING_MIP_LEVELS)
            .baseArrayLayer(0)
            .layerCount(VK_REMAINING_ARRAY_LAYERS)
    }
    private fun createFrameResources(windowSize: Vector2i) {

        (0 until vk.graphics.swapChain.numImages).forEach { i ->

            val targetImage = memory.get(VulkanMemory.DEVICE).allocImage { info ->
                info.imageType(VK_IMAGE_TYPE_2D)
                info.format(VK_FORMAT_R8G8B8A8_UNORM)
                info.mipLevels(1)
                info.usage(VK_IMAGE_USAGE_STORAGE_BIT or VK_IMAGE_USAGE_SAMPLED_BIT)
                info.extent().set(windowSize.x, windowSize.y, 1)
            }.orThrow("Unable to allocate render image")

            descriptors
                .layout(0)
                .createSet()
                .add(sdfObjects.deviceBuffer)
                .add(materialsBuffer.deviceBuffer)
                .add(targetImage.getView(), VK_IMAGE_LAYOUT_GENERAL)
                .add(ubo.deviceBuffer)
                .add(textures.get("brick.dds").image.getView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureSampler!!)
                .write()

            frameResources.add(
                FrameResource(
                    computeCmd = compCommandPool!!.alloc(),
                    computeFinished = context.device.createSemaphore(),
                    computeTarget = targetImage,
                    descriptorSet = descriptors.layout(0).set(i),
                    quad = Quad().init(context, targetImage.getView(), quadSampler!!)
                        .camera(camera2d)
                        .model(Matrix4f().scale(windowSize.x.toFloat(), windowSize.y.toFloat(), 1f))
                )
            )
        }
        log.info("Created ${frameResources.size} compute frame resources")
    }
    private fun cameraChanged() {
        camera3d.V(ubo.view)
        camera3d.invV(ubo.invView)
        ubo.cameraPos.set(camera3d.position)
        ubo.cameraDir.set(camera3d.forward)
        ubo.cameraUp.set(camera3d.up)
        ubo.tanfov2 = Math.tan(camera3d.fov/2.0).toFloat()
        ubo.setStale()
    }
    private fun recordCompute(fr:FrameResource) {

        graphicsToComputeImgBarrier.image(fr.computeTarget.handle)

        fr.computeCmd.let { b->
            b.beginOneTimeSubmit()

            ubo.transfer(b)

            b.bindPipeline(pipeline)

            //b.resetQueryPool(queryPool, index*2, 2)
            //b.writeTimestamp(VPipelineStage.TOP_OF_PIPE, queryPool, index*2)

            b.pushConstants(
                pipeline.layout,
                VK_SHADER_STAGE_COMPUTE_BIT,
                0,
                pushConstants.floatBuffer
            )
            b.bindDescriptorSets(
                VK_PIPELINE_BIND_POINT_COMPUTE,
                pipeline.layout,
                0,
                arrayOf(fr.descriptorSet)
            )
            b.pipelineBarrier(
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                0,      // dependency flags
                null,   // memory barriers
                null,   // buffer barriers
                graphicsToComputeImgBarrier
            )

            b.dispatch(camera2d.windowSize.x/8, camera2d.windowSize.y/8, 1)

            //b.writeTimestamp(VPipelineStage.BOTTOM_OF_PIPE, queryPool, index*2+1);
            b.end()
        }
    }
}