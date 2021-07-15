#include "client.h"
#include "dataset.h"
#include "client_test_utils.h"
#include "dataset_test_utils.h"

void rename_dataset(std::string keyout)
{
    std::vector<size_t> dims({10,10,2});

    SmartRedis::Client client(use_cluster());
    client.use_tensor_ensemble_prefix(true);

    double*** t_send_1 =
        allocate_3D_array<double>(dims[0], dims[1], dims[2]);
    set_3D_array_floating_point_values<double>(t_send_1, dims[0], dims[1], dims[2]);

    double*** t_send_2 =
        allocate_3D_array<double>(dims[0], dims[1], dims[2]);
    set_3D_array_floating_point_values<double>(t_send_2, dims[0], dims[1], dims[2]);

    std::string name = "ensemble_dataset";
    SmartRedis::DataSet dataset(name);

    DATASET_TEST_UTILS::fill_dataset_with_metadata(dataset);

    //Add tensors to the DataSet
    std::string t_name_1 = "tensor_1";
    std::string t_name_2 = "tensor_2";

    dataset.add_tensor(t_name_1, t_send_1, dims,
                       SmartRedis::TensorType::dbl,
                       SmartRedis::MemoryLayout::nested);

    dataset.add_tensor(t_name_2, t_send_2, dims,
                       SmartRedis::TensorType::dbl,
                       SmartRedis::MemoryLayout::nested);


    client.put_dataset(dataset);

    std::string new_name = "ensemble_dataset_renamed";

    client.rename_dataset(name, new_name);

    if(!client.key_exists(keyout + "." + new_name))
        throw std::runtime_error("The dataset ack key for the new "\
                                 "DataSet does not exist in the "
                                 "database.");

    if(client.key_exists(keyout + "." + name))
        throw std::runtime_error("The dataset ack key for the old "\
                                 "DataSet was not deleted.");

    if(client.key_exists(keyout + "." + name + ".meta"))
        throw std::runtime_error("The dataset meta key for the old "\
                                 "DataSet was not deleted.");

    if(client.key_exists(keyout + "." + name + "." + t_name_1))
        throw std::runtime_error("The dataset tensor key for " +
                                 t_name_1 +
                                 " was not deleted.");

    if(client.key_exists(keyout + "." + name + "." + t_name_2))
        throw std::runtime_error("The dataset tensor key for " +
                                 t_name_2 +
                                 " was not deleted.");

    //Retrieving a dataset
    SmartRedis::DataSet retrieved_dataset =
        client.get_dataset(new_name);


    DATASET_TEST_UTILS::check_tensor_names(retrieved_dataset,
                                          {t_name_1, t_name_2});

    //Check that the tensors are the same
    DATASET_TEST_UTILS::check_nested_3D_tensor(retrieved_dataset,
                                               t_name_1,
                                               SmartRedis::TensorType::dbl,
                                               t_send_1, dims);
    DATASET_TEST_UTILS::check_nested_3D_tensor(retrieved_dataset,
                                               t_name_2,
                                               SmartRedis::TensorType::dbl,
                                               t_send_2, dims);

    //Check that the metadata values are correct for the metadata
    DATASET_TEST_UTILS::check_dataset_metadata(retrieved_dataset);

    return;
}

int main(int argc, char* argv[]) {

    const char* old_keyin = std::getenv("SSKEYIN");
    const char* old_keyout = std::getenv("SSKEYOUT");
    char keyin_env_put[] = "SSKEYIN=producer_0,producer_1";
    char keyout_env_put[] = "SSKEYOUT=producer_0";
    putenv( keyin_env_put );
    putenv( keyout_env_put );

    rename_dataset("producer_0");

    if (old_keyin != nullptr) {
        std::string reset_keyin = std::string("SSKEYIN=") + std::string(old_keyin);
        char* reset_keyin_c = new char[reset_keyin.size() + 1];
        std::copy(reset_keyin.begin(), reset_keyin.end(), reset_keyin_c);
        reset_keyin_c[reset_keyin.size()] = '\0';
        putenv( reset_keyin_c);
        delete [] reset_keyin_c;
    }
    else {
        unsetenv("SSKEYIN");
    }

    if (old_keyout != nullptr) {
        std::string reset_keyout = std::string("SSKEYOUT=") + std::string(old_keyout);
        char* reset_keyout_c = new char[reset_keyout.size() + 1];
        std::copy(reset_keyout.begin(), reset_keyout.end(), reset_keyout_c);
        reset_keyout_c[reset_keyout.size()] = '\0';
        putenv( reset_keyout_c);
        delete [] reset_keyout_c;
    }
    else {
        unsetenv("SSKEYOUT");
    }


    std::cout<<"Ensemble test complete"<<std::endl;

    return 0;
}