#include "tensorpack.h"

using namespace SILC;

TensorPack::TensorPack(const TensorPack& tp) {
    /* Copy constructor for TensorPack
    */
    this->_tensors_double = tp._tensors_double;
    this->_tensors_float = tp._tensors_float;
    this->_tensors_int64 = tp._tensors_int64;
    this->_tensors_int32 = tp._tensors_int32;
    this->_tensors_int16 = tp._tensors_int16;
    this->_tensors_int8 = tp._tensors_int8;
    this->_tensors_uint16 = tp._tensors_uint16;
    this->_tensors_uint8 = tp._tensors_uint8;
    this->_rebuild_tensor_inventory();
}

TensorPack& TensorPack::operator=(const TensorPack& tp) {
    /* Copy assignment operator
    */
    if(this!=&tp) {
        this->_tensors_double = tp._tensors_double;
        this->_tensors_float = tp._tensors_float;
        this->_tensors_int64 = tp._tensors_int64;
        this->_tensors_int32 = tp._tensors_int32;
        this->_tensors_int16 = tp._tensors_int16;
        this->_tensors_int8 = tp._tensors_int8;
        this->_tensors_uint16 = tp._tensors_uint16;
        this->_tensors_uint8 = tp._tensors_uint8;
        this->_rebuild_tensor_inventory();
    }
    return *this;
}

void TensorPack::add_tensor(const std::string& name,
                            void* data,
                            const std::vector<size_t>& dims,
                            const TensorType type,
                            const MemoryLayout mem_layout)
{
    /* This function adds a Tensor to the TensorPack.
    */

    if(name.size()==0)
        throw std::runtime_error("The tensor name must "\
                                 "be greater than 0.");

    if(this->tensor_exists(name))
        throw std::runtime_error("The tensor " +
                                 std::string(name) +
                                 " already exists");

    TensorBase* ptr;

    switch(type) {
        case TensorType::dbl :
            ptr = new Tensor<double>(name, data, dims,
                                    type, mem_layout);
            break;
        case TensorType::flt :
            ptr = new Tensor<float>(name, data, dims,
                                    type, mem_layout);
            break;
        case TensorType::int64 :
            ptr = new Tensor<int64_t>(name, data, dims,
                                      type, mem_layout);
            break;
        case TensorType::int32 :
            ptr = new Tensor<int32_t>(name, data, dims,
                                      type, mem_layout);
            break;
        case TensorType::int16 :
            ptr = new Tensor<int16_t>(name, data, dims,
                                     type, mem_layout);
            break;
        case TensorType::int8 :
            ptr = new Tensor<int8_t>(name, data, dims,
                                     type, mem_layout);
            break;
        case TensorType::uint16 :
            ptr = new Tensor<uint16_t>(name, data, dims,
                                       type, mem_layout);
            break;
        case TensorType::uint8 :
             ptr = new Tensor<uint8_t>(name, data, dims,
                                       type, mem_layout);
             break;
    }
    this->add_tensor(ptr);
    return;
}

void TensorPack::add_tensor(TensorBase* tensor)
{
    /* This function will add a tensor that was allocated
    on the heap to the TensorPack inventories to track.
    Note that the TensorBase pointer will be cast
    to Tensor<T> based on the type string in the TensorBase
    object.
    */

    std::string name = tensor->name();

    if(name.size()==0)
        throw std::runtime_error("The tensor name must "\
                                 "be greater than 0.");

    TensorType type =  tensor->type();
    switch(type) {
        case TensorType::dbl :
            this->_tensors_double.add_tensor(tensor);
        break;
        case TensorType::flt :
            this->_tensors_float.add_tensor(tensor);
        break;
        case TensorType::int64 :
            this->_tensors_int64.add_tensor(tensor);
        break;
        case TensorType::int32 :
            this->_tensors_int32.add_tensor(tensor);
        break;
        case TensorType::int16 :
            this->_tensors_int16.add_tensor(tensor);
        break;
        case TensorType::int8 :
            this->_tensors_int8.add_tensor(tensor);
        break;
        case TensorType::uint16 :
            this->_tensors_uint16.add_tensor(tensor);
        break;
        case TensorType::uint8 :
            this->_tensors_uint8.add_tensor(tensor);
        break;
    }
    this->_tensorbase_inventory[name] = tensor;
    this->_all_tensors.push_front(tensor);

    return;
}

TensorBase* TensorPack::get_tensor(const std::string& name)
{
    /* Returns a pointer to the tensor by name
    */
    TensorBase* ptr = this->_tensorbase_inventory.at(name);
    return ptr;
}

void* TensorPack::get_tensor_data(const std::string& name)
{
    /* Returns a pointer to the tensor data
    memory space.
    */
    TensorBase* ptr = this->_tensorbase_inventory.at(name);
    return ptr->data();
}

bool TensorPack::tensor_exists(const std::string& name)
{
    /* Check if a tensor exists by name
    */
    return (this->_tensorbase_inventory.count(name)>0);
}

TensorPack::tensorbase_iterator TensorPack::tensor_begin()
{
    /* Return an iterator to the beginning of the tensors
    */
    return this->_all_tensors.begin();
}

TensorPack::tensorbase_iterator TensorPack::tensor_end()
{
    /* Return an iterator to the past the end tensor
    */
    return this->_all_tensors.end();
}

TensorPack::const_tensorbase_iterator TensorPack::tensor_cbegin()
{
    /* Return a constant iterator to the beginning of tensors
    */
    return this->_all_tensors.cbegin();
}

TensorPack::const_tensorbase_iterator TensorPack::tensor_cend()
{
    /* return a constant iterator to the past the end tensor
    */
    return this->_all_tensors.cend();
}

void TensorPack::_rebuild_tensor_inventory() {
    /* This function will rebuild the tensor inventories.
    */
    this->_all_tensors.clear();
    this->_tensorbase_inventory.clear();
    this->_add_tensorlist_to_inventory<double>(this->_tensors_double);
    this->_add_tensorlist_to_inventory<float>(this->_tensors_float);
    this->_add_tensorlist_to_inventory<int64_t>(this->_tensors_int64);
    this->_add_tensorlist_to_inventory<int32_t>(this->_tensors_int32);
    this->_add_tensorlist_to_inventory<int16_t>(this->_tensors_int16);
    this->_add_tensorlist_to_inventory<int8_t>(this->_tensors_int8);
    this->_add_tensorlist_to_inventory<uint16_t>(this->_tensors_uint16);
    this->_add_tensorlist_to_inventory<uint8_t>(this->_tensors_uint8);
    return;
}

template <typename T>
void TensorPack::_add_tensorlist_to_inventory(TensorList<T>& t_list) {
    /* This function will add a TensorList<T> to the
    tensor inventory.
    */
    typename TensorList<T>::iterator it =
        t_list.begin();
    typename TensorList<T>::iterator it_end =
        t_list.end();

    while(it!=it_end) {
        this->_all_tensors.push_front((TensorBase*)(*it));
        this->_tensorbase_inventory[(*it)->name()] = (TensorBase*)(*it);
        it++;
    }
    return;

}

