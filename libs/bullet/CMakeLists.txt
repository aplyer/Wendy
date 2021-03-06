
project(bullet CXX)

set(bullet_HEADERS
    btAabbUtil2.h
    btActionInterface.h
    btActivatingCollisionAlgorithm.h
    btAlignedAllocator.h
    btAlignedObjectArray.h
    btAxisSweep3.h
    btBox2dBox2dCollisionAlgorithm.h
    btBox2dShape.h
    btBoxBoxCollisionAlgorithm.h
    btBoxBoxDetector.h
    btBoxCollision.h
    btBoxShape.h
    btBroadphaseInterface.h
    btBroadphaseProxy.h
    btBulletCollisionCommon.h
    btBulletDynamicsCommon.h
    btBvhTriangleMeshShape.h
    btCapsuleShape.h
    btClipPolygon.h
    btCollisionAlgorithm.h
    btCollisionConfiguration.h
    btCollisionCreateFunc.h
    btCollisionDispatcher.h
    btCollisionMargin.h
    btCollisionObject.h
    btCollisionShape.h
    btCollisionWorld.h
    btCompoundCollisionAlgorithm.h
    btCompoundShape.h
    btConcaveShape.h
    btConeShape.h
    btConeTwistConstraint.h
    btConstraintSolver.h
    btContactConstraint.h
    btContactProcessing.h
    btContactSolverInfo.h
    btContinuousConvexCollision.h
    btConvex2dConvex2dAlgorithm.h
    btConvex2dShape.h
    btConvexCast.h
    btConvexConcaveCollisionAlgorithm.h
    btConvexConvexAlgorithm.h
    btConvexHullComputer.h
    btConvexHull.h
    btConvexHullShape.h
    btConvexInternalShape.h
    btConvexPenetrationDepthSolver.h
    btConvexPlaneCollisionAlgorithm.h
    btConvexPointCloudShape.h
    btConvexPolyhedron.h
    btConvexShape.h
    btConvexTriangleMeshShape.h
    btCylinderShape.h
    btDbvtBroadphase.h
    btDbvt.h
    btDefaultCollisionConfiguration.h
    btDefaultMotionState.h
    btDiscreteCollisionDetectorInterface.h
    btDiscreteDynamicsWorld.h
    btDispatcher.h
    btDynamicsWorld.h
    btEmptyCollisionAlgorithm.h
    btEmptyShape.h
    btGeneric6DofConstraint.h
    btGeneric6DofSpringConstraint.h
    btGenericPoolAllocator.h
    btGeometryOperations.h
    btGeometryUtil.h
    btGhostObject.h
    btGImpactBvh.h
    btGImpactCollisionAlgorithm.h
    btGImpactMassUtil.h
    btGImpactQuantizedBvh.h
    btGImpactShape.h
    btGjkConvexCast.h
    btGjkEpa2.h
    btGjkEpaPenetrationDepthSolver.h
    btGjkPairDetector.h
    btGrahamScan2dConvexHull.h
    btHashMap.h
    btHeightfieldTerrainShape.h
    btHinge2Constraint.h
    btHingeConstraint.h
    btIDebugDraw.h
    btInternalEdgeUtility.h
    btJacobianEntry.h
    btList.h
    btManifoldPoint.h
    btManifoldResult.h
    btMaterial.h
    btMatrix3x3.h
    btMinkowskiPenetrationDepthSolver.h
    btMinkowskiSumShape.h
    btMinMax.h
    btMotionState.h
    btMultimaterialTriangleMeshShape.h
    btMultiSapBroadphase.h
    btMultiSphereShape.h
    btOptimizedBvh.h
    btOverlappingPairCache.h
    btOverlappingPairCallback.h
    btPersistentManifold.h
    btPoint2PointConstraint.h
    btPointCollector.h
    btPolyhedralContactClipping.h
    btPolyhedralConvexShape.h
    btPoolAllocator.h
    btQuadWord.h
    btQuantization.h
    btQuantizedBvh.h
    btQuaternion.h
    btQuickprof.h
    btRandom.h
    btRaycastCallback.h
    btRigidBody.h
    btScalar.h
    btScaledBvhTriangleMeshShape.h
    btSequentialImpulseConstraintSolver.h
    btSerializer.h
    btShapeHull.h
    btSimpleBroadphase.h
    btSimpleDynamicsWorld.h
    btSimplexSolverInterface.h
    btSimulationIslandManager.h
    btSliderConstraint.h
    btSolve2LinearConstraint.h
    btSolverBody.h
    btSolverConstraint.h
    btSphereBoxCollisionAlgorithm.h
    btSphereShape.h
    btSphereSphereCollisionAlgorithm.h
    btSphereTriangleCollisionAlgorithm.h
    btStackAlloc.h
    btStaticPlaneShape.h
    btStridingMeshInterface.h
    btSubSimplexConvexCast.h
    btTetrahedronShape.h
    btTransform.h
    btTransformUtil.h
    btTriangleBuffer.h
    btTriangleCallback.h
    btTriangleIndexVertexArray.h
    btTriangleIndexVertexMaterialArray.h
    btTriangleInfoMap.h
    btTriangleMesh.h
    btTriangleMeshShape.h
    btTriangleShapeEx.h
    btTriangleShape.h
    btTypedConstraint.h
    btUniformScalingShape.h
    btUnionFind.h
    btUniversalConstraint.h
    btVector3.h
    btVoronoiSimplexSolver.h
    gim_array.h
    gim_basic_geometry_operations.h
    gim_bitset.h
    gim_box_collision.h
    gim_box_set.h
    gim_clip_polygon.h
    gim_contact.h
    gim_geometry.h
    gim_geom_types.h
    gim_hash_table.h
    gim_linear_math.h
    gim_math.h
    gim_memory.h
    gim_radixsort.h
    gim_tri_collision.h
    SphereTriangleDetector.h)

set(bullet_SOURCES 
    btActivatingCollisionAlgorithm.cpp
    btAlignedAllocator.cpp
    btAxisSweep3.cpp
    btBox2dBox2dCollisionAlgorithm.cpp
    btBox2dShape.cpp
    btBoxBoxCollisionAlgorithm.cpp
    btBoxBoxDetector.cpp
    btBoxShape.cpp
    btBroadphaseProxy.cpp
    btBvhTriangleMeshShape.cpp
    btCapsuleShape.cpp
    btCollisionAlgorithm.cpp
    btCollisionDispatcher.cpp
    btCollisionObject.cpp
    btCollisionShape.cpp
    btCollisionWorld.cpp
    btCompoundCollisionAlgorithm.cpp
    btCompoundShape.cpp
    btConcaveShape.cpp
    btConeShape.cpp
    btConeTwistConstraint.cpp
    btContactConstraint.cpp
    btContactProcessing.cpp
    btContinuousConvexCollision.cpp
    btConvex2dConvex2dAlgorithm.cpp
    btConvex2dShape.cpp
    btConvexCast.cpp
    btConvexConcaveCollisionAlgorithm.cpp
    btConvexConvexAlgorithm.cpp
    btConvexHullComputer.cpp
    btConvexHull.cpp
    btConvexHullShape.cpp
    btConvexInternalShape.cpp
    btConvexPlaneCollisionAlgorithm.cpp
    btConvexPointCloudShape.cpp
    btConvexPolyhedron.cpp
    btConvexShape.cpp
    btConvexTriangleMeshShape.cpp
    btCylinderShape.cpp
    btDbvtBroadphase.cpp
    btDbvt.cpp
    btDefaultCollisionConfiguration.cpp
    btDiscreteDynamicsWorld.cpp
    btDispatcher.cpp
    btEmptyCollisionAlgorithm.cpp
    btEmptyShape.cpp
    btGeneric6DofConstraint.cpp
    btGeneric6DofSpringConstraint.cpp
    btGenericPoolAllocator.cpp
    btGeometryUtil.cpp
    btGhostObject.cpp
    btGImpactBvh.cpp
    btGImpactCollisionAlgorithm.cpp
    btGImpactQuantizedBvh.cpp
    btGImpactShape.cpp
    btGjkConvexCast.cpp
    btGjkEpa2.cpp
    btGjkEpaPenetrationDepthSolver.cpp
    btGjkPairDetector.cpp
    btHeightfieldTerrainShape.cpp
    btHinge2Constraint.cpp
    btHingeConstraint.cpp
    btInternalEdgeUtility.cpp
    btManifoldResult.cpp
    btMinkowskiPenetrationDepthSolver.cpp
    btMinkowskiSumShape.cpp
    btMultimaterialTriangleMeshShape.cpp
    btMultiSphereShape.cpp
    btOptimizedBvh.cpp
    btOverlappingPairCache.cpp
    btPersistentManifold.cpp
    btPoint2PointConstraint.cpp
    btPolyhedralContactClipping.cpp
    btPolyhedralConvexShape.cpp
    btQuantizedBvh.cpp
    btQuickprof.cpp
    btRaycastCallback.cpp
    btRigidBody.cpp
    btScaledBvhTriangleMeshShape.cpp
    btSequentialImpulseConstraintSolver.cpp
    btSerializer.cpp
    btShapeHull.cpp
    btSimulationIslandManager.cpp
    btSliderConstraint.cpp
    btSolve2LinearConstraint.cpp
    btSphereBoxCollisionAlgorithm.cpp
    btSphereShape.cpp
    btSphereSphereCollisionAlgorithm.cpp
    btSphereTriangleCollisionAlgorithm.cpp
    btStaticPlaneShape.cpp
    btStridingMeshInterface.cpp
    btSubSimplexConvexCast.cpp
    btTetrahedronShape.cpp
    btTriangleBuffer.cpp
    btTriangleCallback.cpp
    btTriangleIndexVertexArray.cpp
    btTriangleIndexVertexMaterialArray.cpp
    btTriangleMesh.cpp
    btTriangleMeshShape.cpp
    btTriangleShapeEx.cpp
    btTypedConstraint.cpp
    btUniformScalingShape.cpp
    btUnionFind.cpp
    btUniversalConstraint.cpp
    btVoronoiSimplexSolver.cpp
    gim_box_set.cpp
    gim_contact.cpp
    gim_memory.cpp
    gim_tri_collision.cpp
    SphereTriangleDetector.cpp)

add_library(bullet STATIC ${bullet_SOURCES} ${bullet_HEADERS})

