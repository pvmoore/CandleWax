package candlewax

/**
 * Things to do:
 *
 * Modify scene.inc to take an array of object properties from the dataIn buffer. Use
 * these properties to display different sdf objects. I expect this to be quite slow but
 * try it to see what the baseline performance is.
 *
 *
 * Run several shader passes (one for each type of object). Each pass adds hit depth, material,
 * normal and occlusion data to the result buffer.
 * This may not work well with soft shadows? Or maybe it will be ok.
 *
 *
 *
 *
 * Generate GLSL scene file dynamically as the scene changes. This would require a new pipeline
 * every time the shader file changes.
 *
 *
 *
 *
 *
 * Optimisation ideas:
 *
 *  Bounding boxes/spheres:
 *
 *      Use axis-aligned bounding boxes in scene for complex objects.
 *      Bounding spheres may work better.
 *
 *  Distance sentinel array acceleration structure:
 *
 *      Static sentinels:
 *
 *      Create a float array which maintains scene distances from each array point. Use the nearest
 *      point when marching. If ray pos is inside a sentinel's distance field then it can proceed
 *      quickly to the outer edge of that field before checking again for another sentinel. If
 *      it can't proceed any further using that method then fallback to the standard method until
 *      it can use the sentinels again or it has hit an object.
 *
 *      More finely-grained sentinel arrays can be used too but may nor offer much benefit. Testing is required.
 *
 *      This method should work well for static scenes. For dynamic objects, a second array will be required
 *      (see Dynamic Sentinels).
 *
 *      Dynamic sentinels:
 *
 *      These will need to be updated whenever any dynamic object moves.
 *
 *      Observation:
 *
 *      Both static and dynamic sentinel distance fields only need to use the bounding box/sphere of
 *      the scene objects. This should make them faster to update (especially the dynamic ones).
 *
 *  Object information grid acceleration structure:
 *
 *      Create an array grid of equally sized boxes. Each grid box contains information about the
 *      objects that are entirely or partially inside it.
 *      When ray marching, use this grid to speed up the march by only checking objects that are in
 *      the same grid as the ray. Move forward by the minimum of the grid objects minimum or the grid
 *      exit, whichever is smaller.
 *
 *      This grid can be an octree so a grid cell's parent can also be checked if this is faster.
 *
 *      Each empty grid cell can have a distance to the next non-empty cell.
 */
