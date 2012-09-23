#include "GpuCloud.H"

static void CL_CALLBACK errorCallback(const char* errinfo, const void* private_info, size_t cb, void* user_data) 
{
    string skip = "OpenCL Build Warning : Compiler build log:";
    if (strncmp(errinfo, skip.c_str(), skip.length()) == 0)
        return; // OS X Lion insists on calling this for every build warning, even though they aren't errors.
    std::cerr << "OpenCL internal error: " << errinfo << std::endl;
}
Foam::GpuCloud::GpuCloud()
{
	std::cout<<"Gpu Cloud Called\n";
	vector<cl::Platform> platforms;
	vector<cl::Device> devices,allDevices,ctxDevices;
	try
	{
		cl::Platform::get(&platforms);
		int processingElementsPerComputeUnit = 8;
		this->deviceIndex = 0;
		if(platforms.size()<=0)
		{
			std::cout<<"No OpenCL platform found, quiting ..."<<endl;
		}
		cl_context_properties cprops[] = {CL_CONTEXT_PLATFORM, (cl_context_properties) platforms[0](), 0};
	        context = cl::Context(CL_DEVICE_TYPE_GPU, cprops, errorCallback);
	        vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
	        if(devices.size()<=0){
			std::cout<<"No GPU devices found for this platform."<<endl;
			exit(0);
		}	
		if(platforms[0].getInfo<CL_PLATFORM_VENDOR>().find("NVIDIA")!=string::npos)
		{
			std::cout<<"Nvidia platform found"<<endl;
			if(devices[this->deviceIndex].getInfo<CL_DEVICE_EXTENSIONS>().find("cl_nv_device_attribute_query") != string::npos)
			{
				 cl_uint computeCapabilityMajor;
  		                 clGetDeviceInfo(devices[this->deviceIndex](), CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV, 
						 sizeof(cl_uint), &computeCapabilityMajor, NULL);
				 processingElementsPerComputeUnit = (computeCapabilityMajor < 2 ? 8 : 32);
				 int workGroupSize = devices[this->deviceIndex].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
				 this->defaultOptimisationOptions = "-cl-fast-relaxed-math";
				 this->supports64BitGlobalAtomics = true;
				 this->supportsDoublePrecision = (devices[this->deviceIndex].getInfo<CL_DEVICE_EXTENSIONS>().find("cl_khr_fp64") != string::npos);
				 compilationDefines["SQRT"] = "native_sqrt";
				 compilationDefines["RSQRT"] = "native_rsqrt";
				 compilationDefines["RECIP"] = "native_recip";
				 compilationDefines["EXP"] = "native_exp";
				 compilationDefines["LOG"] = "native_log";
			}
		}
		else
		{
			std::cout<<"Some other platform found"<<endl;
			exit(0);
			/**
			 * todo:optimizations for other gpu platforms
			 * must be done later in second stage however till that 
			 * time the program should exit if it fails to find nvidia gpus
			 */
		}
	}
	catch(cl::Error e)
	{
		cout<<e.what()<<" : Error code "
			<<e.err()<<endl;
	}


}

Foam::GpuCloud::~GpuCloud()
{}

void Foam::GpuCloud::initialise()
{
	std::cout<<"I am invoked inside GpuCloud\n";
   vector<cl::Platform> platforms;
   vector<cl::Device> devicess;
   vector<cl::Kernel> allKernels;
   std::string kernelName;

   try {
      // Place the GPU devices of the first platform into a context
      cl::Platform::get(&platforms);
      platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devicess);
      cl::Context cont(devicess);

      // Create and build program
      //std::ifstream programFile("/home/ksb12101/OpenFOAM/OpenFOAM-Dev/src/lagrangian/dsmc/gpucloud/kernels/kernels.cl");
      std::ifstream programFile("/kernels/kernels.cl");
      std::string programString(std::istreambuf_iterator<char>(programFile),
            (std::istreambuf_iterator<char>()));
      cl::Program::Sources src(1, std::make_pair(programString.c_str(),
            programString.length()+1));
      cl::Program program(cont, src);
      program.build(devicess);

      // Create individual kernels
      cl::Kernel addKernel(program, "add");
      cl::Kernel subKernel(program, "subtract");
      cl::Kernel multKernel(program, "multiply");

      // Create all kernels in program
      program.createKernels(&allKernels);
      for(unsigned int i=0; i<allKernels.size(); i++) {
         kernelName = allKernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>();
         std::cout << "Kernel: " << kernelName << std::endl;
      }
   }
   catch(cl::Error e) {
      std::cout << e.what() << ": Error code " << e.err() << std::endl;
   }

}

std::string Foam::GpuCloud::loadSourceFromFile(string fileName)
{
//	string file = ("/home/ksb12101/OpenFOAM/OpenFOAM-Dev/src/lagrangian/dsmc/gpucloud/kernels/vectorkernel.cl").c_str();
	std::ifstream sourceFile("/home/ksb12101/OpenFOAM/OpenFOAM-Dev/src/lagrangian/dsmc/gpucloud/kernels/vectorkernel.cl");
        if(!sourceFile.is_open())
                cout<<"File not found \n";
        else
                cout<<"Fille found \n";
        string kernel;
        string line;
        while(!sourceFile.eof())
        {
                getline(sourceFile,line);
                kernel += line;
                kernel += "\n";
        }
        sourceFile.close();
	return kernel;
}
