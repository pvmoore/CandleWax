package candlewax

import org.apache.log4j.Logger
import org.lwjgl.vulkan.VkPhysicalDeviceFeatures
import org.lwjgl.vulkan.VkQueueFamilyProperties
import vulkan.app.VulkanApplication
import vulkan.common.FrameInfo
import vulkan.common.PerFrameResource
import vulkan.common.Queues
import vulkan.common.VulkanClient

fun main(args: Array<String>) {
    CandleWax.run()
    CandleWax.destroy()
}

object CandleWax : VulkanClient(Parameters(
    windowTitle             = "CandleWax $VERSION",
    windowed                = true,
    enableVsync             = false,
    width                   = 1000,
    height                  = 800,
    prefNumSwapChainBuffers = 2))
{
    val log = Logger.getLogger(this::class.java)

    private val vk     = VulkanApplication(this)
    private val scene  = Scene()

    init{
        vk.initialise()
    }
    override fun destroy() {
        scene.destroy()
        vk.destroy()
    }
    override fun enableFeatures(f: VkPhysicalDeviceFeatures) {
        f.geometryShader(true)
    }
    override fun selectQueues(props: VkQueueFamilyProperties.Buffer, queues: Queues) {
        super.selectQueues(props, queues)

        var found = false

        /** Select the first compute queue that we see */
        props.forEachIndexed { i, family ->
            if(family.queueCount() > 0) {

                if(queues.isCompute(family.queueFlags())) {

                    /** Prefer non-graphics compute queue */
                    if(!found || !queues.isGraphics(family.queueFlags())) {
                        queues.select(Queues.COMPUTE, i, 1)
                        found = true
                    }
                }
            }
        }
    }
    override fun vulkanReady(vk: VulkanApplication) {

        vk.shaders.setCompilerPath("/pvmoore/_tools/glslangValidator.exe")

        scene.init(vk)

        vk.graphics.showWindow()
    }
    fun run() {
        vk.graphics.enterLoop()
    }
    override fun render(frame: FrameInfo, res: PerFrameResource) {
        scene.update(frame, res)
        scene.render(frame, res)
    }
}