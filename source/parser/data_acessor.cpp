#include "data_acessor.hpp"

#include <vector>

#include "types.hpp"
#include "utils.hpp"

DataAcessor read_acessor_data(
    ConstJsonArray buffer_views,
    const std::vector<std::vector<char> >& buffers_contents,
    const rapidjson::Value& acessor
) {
    int buffer_view_index = acessor["bufferView"].GetInt();
    int component_type = acessor["componentType"].GetInt();
    int count = acessor["count"].GetInt();
    int bytes_count = get_size_in_bytes(component_type);

    std::string type = acessor["type"].GetString();
    size_t components_count = get_type_number_of_compoents(type);

    int byte_offset = 0;
    if (acessor.HasMember("byteOffset")) {
        byte_offset = acessor["byteOffset"].GetInt();
    }

    const rapidjson::Value& buffer_view = buffer_views[buffer_view_index];
    int buffer_index = buffer_view["buffer"].GetInt();

    int start = byte_offset + buffer_view["byteOffset"].GetInt();

    auto it_start = buffers_contents[buffer_index].begin() + start;
    auto it_end = it_start + count * bytes_count * components_count;

    return {std::vector<char> (it_start, it_end), type, component_type};
}
