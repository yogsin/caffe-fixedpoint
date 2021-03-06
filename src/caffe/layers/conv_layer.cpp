#include <vector>
#include <iostream>
#include "caffe/layers/conv_layer.hpp"
float max_conv=0;
float max_in=0;
int count_conv=0;
namespace caffe {

template <typename Dtype>
void ConvolutionLayer<Dtype>::compute_output_shape() {
  const int* kernel_shape_data = this->kernel_shape_.cpu_data();
  const int* stride_data = this->stride_.cpu_data();
  const int* pad_data = this->pad_.cpu_data();
  const int* dilation_data = this->dilation_.cpu_data();
  this->output_shape_.clear();
  for (int i = 0; i < this->num_spatial_axes_; ++i) {
    // i + 1 to skip channel axis
    const int input_dim = this->input_shape(i + 1);
    const int kernel_extent = dilation_data[i] * (kernel_shape_data[i] - 1) + 1;
    const int output_dim = (input_dim + 2 * pad_data[i] - kernel_extent)
        / stride_data[i] + 1;
    this->output_shape_.push_back(output_dim);
  }
}

template <typename Dtype>
void ConvolutionLayer<Dtype>::Forward_cpu(const vector<Blob<Dtype>*>& bottom,
      const vector<Blob<Dtype>*>& top) {
  const Dtype* weight = this->blobs_[0]->cpu_data();
  if(count_conv<2){
       for (int i = 0; i < bottom.size(); ++i){
         Dtype* bottom_data_mutable = bottom[i]->mutable_cpu_data();
         for(int j=0;j<this->num_*this->bottom_dim_;j++)
           bottom_data_mutable[j]=bottom_data_mutable[j]*0.01;
       }
       Dtype* bias_mutable = this->blobs_[1]->mutable_cpu_data();
      for(int i=0;i<this->blobs_[1]->shape(0);i++){
        bias_mutable[i]=bias_mutable[i]*0.01;
      }
      count_conv++;
  }
  for (int i = 0; i < bottom.size(); ++i) {
    const Dtype* bottom_data = bottom[i]->cpu_data();
    //Dtype* bottom_data_mutable=
    Dtype* top_data = top[i]->mutable_cpu_data();
    //std::cout<<"num_:"<<this->num_<<std::endl;
    //std::cout<<"bottom_dim_:"<<this->bottom_dim_<<std::endl;
    //std::cout<<"top_dim_:"<<this->top_dim_<<std::endl;
    for (int n = 0; n < this->num_; ++n) {
      this->forward_cpu_gemm(bottom_data + n * this->bottom_dim_, weight,
          top_data + n * this->top_dim_);
      if (this->bias_term_) {
        const Dtype* bias = this->blobs_[1]->cpu_data();
      for (int j = 0; j < 10; ++j)
        std::cout<<"bias:"<<bias[j]<<std::endl;
        this->forward_cpu_bias(top_data + n * this->top_dim_, bias);
      }
    }
     for (int n = 0; n < 1; ++n) {
      std::cout<<"bottom:"<<std::endl;
      for(int j =0; j < 10; j++)
        std::cout<<*(bottom_data+n*this->bottom_dim_+j)<<" ";
      std::cout<<"top:"<<std::endl;
      for(int j =0; j < 10; j++)
        std::cout<<top_data[n*this->top_dim_+j]<<" ";
      for(int j=0;j<this->num_*this->top_dim_;j++)
        if(top_data[j]>max_conv) max_conv=float(top_data[j]);
      for(int j=0;j<this->num_*this->bottom_dim_;j++)
          if(bottom_data[j]>max_in) max_in=float(bottom_data[j]);
      std::cout<<std::endl;
      std::cout<<"conv max:"<<max_conv<<std::endl;
      std::cout<<std::endl;
      std::cout<<"conv in:"<<max_in<<std::endl;

    }

  }
}
/*
template <typename Dtype>
void ConvolutionLayer<Dtype>::Backward_cpu(const vector<Blob<Dtype>*>& top,
      const vector<bool>& propagate_down, const vector<Blob<Dtype>*>& bottom) {
  const Dtype* weight = this->blobs_[0]->cpu_data();
  Dtype* weight_diff = this->blobs_[0]->mutable_cpu_diff();
  for (int i = 0; i < top.size(); ++i) {
    const Dtype* top_diff = top[i]->cpu_diff();
    const Dtype* bottom_data = bottom[i]->cpu_data();
    Dtype* bottom_diff = bottom[i]->mutable_cpu_diff();
    // Bias gradient, if necessary.
    if (this->bias_term_ && this->param_propagate_down_[1]) {
      Dtype* bias_diff = this->blobs_[1]->mutable_cpu_diff();
      for (int n = 0; n < this->num_; ++n) {
        this->backward_cpu_bias(bias_diff, top_diff + n * this->top_dim_);
      }
    }
    if (this->param_propagate_down_[0] || propagate_down[i]) {
      for (int n = 0; n < this->num_; ++n) {
        // gradient w.r.t. weight. Note that we will accumulate diffs.
        if (this->param_propagate_down_[0]) {
          this->weight_cpu_gemm(bottom_data + n * this->bottom_dim_,
              top_diff + n * this->top_dim_, weight_diff);
        }
        // gradient w.r.t. bottom data, if necessary.
        if (propagate_down[i]) {
          this->backward_cpu_gemm(top_diff + n * this->top_dim_, weight,
              bottom_diff + n * this->bottom_dim_);
        }
      }
    }
  }
}
*/


INSTANTIATE_CLASS(ConvolutionLayer);

}  // namespace caffe
