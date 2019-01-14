
__kernel void sigmoid_vec(__global double *input, __global double *output)
{
  uint idx = get_global_id(0);
  double x = input[idx];
  output[idx] = 1.0/(1.0 + exp(-x));
}