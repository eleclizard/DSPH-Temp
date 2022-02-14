cmake_minimum_required(VERSION 2.8)

PROJECT(DualSPHysics)

set(OBJ_BASIC main.cpp Functions.cpp FunctionsMath.cpp JArraysCpu.cpp JBinaryData.cpp JCellDivCpu.cpp JCfgRun.cpp JException.cpp JLog2.cpp JObject.cpp JPartDataBi4.cpp JPartFloatBi4.cpp JPartOutBi4Save.cpp JPartsOut.cpp JRadixSort.cpp JRangeFilter.cpp JReadDatafile.cpp JSaveDt.cpp JSpaceCtes.cpp JSpaceEParms.cpp JSpaceParts.cpp JSpaceProperties.cpp JSph.cpp JSphAccInput.cpp JSphCpu.cpp JSphDtFixed.cpp JSphVisco.cpp randomc.cpp JTimeOut.cpp)
set(OBJ_CPU_SINGLE JCellDivCpuSingle.cpp JSphCpuSingle.cpp JPartsLoad4.cpp)
set(OBJ_GPU JArraysGpu.cpp JCellDivGpu.cpp JObjectGpu.cpp JSphGpu.cpp JBlockSizeAuto.cpp JMeanValues.cpp)
set(OBJ_GPU_SINGLE JCellDivGpuSingle.cpp JSphGpuSingle.cpp)

set(OBJ_CUDA JCellDivGpu_ker.cu JSphGpu_ker.cu)
set(OBJ_CUDA_SINGLE JCellDivGpuSingle_ker.cu)


find_package(CUDA QUIET REQUIRED)

if(CUDA_VERSION VERSION_LESS "7.5")
  message("Using cuda version <7.5")
  list(APPEND CUDA_NVCC_FLAGS "-use_fast_math -O3 -gencode=arch=compute_20,code=\"sm_20,compute_20\"")
else()
  message("Using cuda version >=7.5")
  list(APPEND CUDA_NVCC_FLAGS "-use_fast_math -O3  -gencode=arch=compute_20,code=\"sm_20,compute_20\" -gencode=arch=compute_30,code=\"sm_30,compute_30\" -gencode=arch=compute_35,code=\"sm_35,compute_35\" -gencode=arch=compute_37,code=\"sm_37,compute_37\" -gencode=arch=compute_50,code=\"sm_50,compute_50\" -gencode=arch=compute_52,code=\"sm_52,compute_52\"")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message("Using libraries for gcc")
    LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
elseif(MSVC)
  message("Windows version")

  LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../Libs)


  #if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "your.required.msvc.version")
  #  message(FATAL_ERROR "Insufficient msvc version")
  #endif()
endif()

find_package(OpenMP)

if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_executable(dualsphysics4cpu ${OBJ_BASIC} ${OBJ_CPU_SINGLE})
cuda_add_executable(dualsphysics4gpu ${OBJ_BASIC} ${OBJ_CPU_SINGLE} ${OBJ_GPU} ${OBJ_CUDA} ${OBJ_GPU_SINGLE} ${OBJ_CUDA_SINGLE})

install(TARGETS dualsphysics4cpu dualsphysics4gpu DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../EXECS)
	
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(dualsphysics4cpu jxml_64 jformatfiles2_64 jsphmotion_64 jwavegen_64)
  target_link_libraries(dualsphysics4gpu jxml_64 jformatfiles2_64 jsphmotion_64 jwavegen_64)
  set_target_properties(dualsphysics4cpu PROPERTIES COMPILE_FLAGS "-use_fast_math -O3")	
  set_target_properties(dualsphysics4gpu PROPERTIES COMPILE_FLAGS "-use_fast_math -O3 -D_WITHGPU")	
elseif(MSVC)
  set_target_properties(dualsphysics4gpu PROPERTIES COMPILE_FLAGS "/D _WITHGPU")

  # MSVC 10
  if(MSVC_VERSION VERSION_EQUAL 1600)
    target_link_libraries(dualsphysics4cpu JXml_x64_v100_Release JFormatFiles2_x64_v100_Release JSphMotion_x64_v100_Release JWaveGen_x64_v100_Release)
    target_link_libraries(dualsphysics4gpu JXml_x64_v100_Release JFormatFiles2_x64_v100_Release JSphMotion_x64_v100_Release JWaveGen_x64_v100_Release)
  elseif(MSVC_VERSION VERSION_EQUAL 1800) 
    target_link_libraries(dualsphysics4cpu JXml_x64_v120_Release JFormatFiles2_x64_v120_Release JSphMotion_x64_v120_Release JWaveGen_x64_v120_Release)
    target_link_libraries(dualsphysics4gpu JXml_x64_v120_Release JFormatFiles2_x64_v120_Release JSphMotion_x64_v120_Release JWaveGen_x64_v120_Release)
  endif()  
  
  SET(CUDA_PROPAGATE_HOST_FLAGS OFF CACHE BOOL "Propagate C/CXX Flags and friends to the host compiler in NVCC via -Xompile  " FORCE)
  
  foreach(CPP_FLAGS CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    if(${CPP_FLAGS} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${CPP_FLAGS} "${${CPP_FLAGS}}")
    endif(${CPP_FLAGS} MATCHES "/MD")
  endforeach(CPP_FLAGS)
endif()
 
