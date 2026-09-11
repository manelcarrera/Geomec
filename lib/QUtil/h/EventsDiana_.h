#pragma once

//
// diana msgs
//
enum eDianaMsg { Gen, AppSpc, Error, Warning, StopExe, Finished, NumMsg };
static const std::vector<QString> g_diana_msg_type = {"G", "A", "E", "W", "S", "F"};
static const std::map<QString, eDianaMsg> g_diana_msg_type_map = {{"G", Gen},     {"A", AppSpc},  {"E", Error},
                                                                  {"W", Warning}, {"S", StopExe}, {"F", Finished}};