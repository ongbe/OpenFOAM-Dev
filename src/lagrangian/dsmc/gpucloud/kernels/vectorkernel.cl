__kernel vectorKernel(global float* vec1)
{
	unsigned int i = get_global_id(0);
	vec1[i] = (float) i;
}
