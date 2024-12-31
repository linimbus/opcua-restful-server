#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(listen_address, "0.0.0.0",
              "The opcua restful server listen to address");

DEFINE_uint32(listen_port, 3344, "The opcua restful Server listen to port");

DEFINE_string(https_ca_file, "",
              "The opcua restful server ca certificate file in PEM format");

DEFINE_string(https_cert_file, "",
              "The opcua restful server certificate file in PEM format");

DEFINE_string(https_key_file, "",
              "The opcua restful server private key file in PEM format");

int main(int argc, char **argv) {
  gflags::SetUsageMessage("The Usage for opcua restful server.");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_log_dir == "") {
    FLAGS_alsologtostderr = true;
  }
  google::InitGoogleLogging(argv[0]);

  //   arrow::Status st = flightsql::server::RunServerMain();
  //   if (!st.ok()) {
  //     std::cerr << st << std::endl;
  //   }

  google::ShutdownGoogleLogging();
  return 0;
}
