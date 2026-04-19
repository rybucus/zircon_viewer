#pragma once

#include <utils/json.hxx>

namespace zircon::app
{

inline const utils::json_t k_application_config = nlohmann::json::parse
(
    R"json(
    {

    }
    )json"
);

} // namespace zircon::app
