#ifndef GpuCloud_H
#define GpuCloud_H
#define __CL_ENABLE_EXCEPTIONS

/*#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif*/
#include <stdio.h>
#include <stdlib.h>
#include <cl.hpp>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#define FILESRC "vectorkernel.h"
using namespace std;
/**
 * @Class: Foam::GpuCloud
 *
 *
 * @description: This is a base class for the gpu implementation
 * inside OpenCL
 *
 * SourceFiles
 * 	GpuCloud.C
 */
namespace Foam 
{
	class GpuCloud
	{
		private:
			int deviceIndex;
			int SIMDWidth;
			bool supports64BitGlobalAtomics;
			bool supportsDoublePrecision;
			string defaultOptimisationOptions;
			std::map<std::string,std::string> compilationDefines;
			cl::Context context;
			cl::Device device;
			cl::CommandQueue queue;
			cl::Program program;
			cl::Kernel firstKernel;
		public:
		//Constructors
		GpuCloud();
		//Destructors
		~GpuCloud();
		void initialise();
		/**
		 * get device index if multiple devices are found
		 */
		int getDeviceIndex()
		{
			return deviceIndex;
		}
		/**
		 * get SIMDWidth for the selected device
		 */
		int getSIMDWidth()
		{
			return SIMDWidth;
		}
		/**
		 * get boolean if 64 bits global atomics are supported
		 */
		bool getSupports64BitsGlobalAtomics()
		{
			return supports64BitGlobalAtomics;
		}
		/**
		 * check if the selected device supports double precision
		 */
		bool getSupportsDoublePrecision()
		{
			return supportsDoublePrecision;
		}
		std::string loadSourceFromFile(std::string fileName);

	};
}//end of foam namespace
#endif
