# Etcd provider demo

This example demonstrates how to use etcd to store and retrieve key-value pairs in a client and server program with Etcd. The example includes a simple client and server program.

## File Structure

```shell
$ tree examples
examples
├── client
│   ├── BUILD
│   ├── client.cc
│   └── trpc_cpp_fiber.yaml
└── server
    ├── BUILD
    ├── greeter_service.cc
    ├── greeter_service.h
    ├── helloworld.proto
    ├── helloworld_server.cc
    └── trpc_cpp_fiber.yaml
└── run.sh
```
## Running the Example

* Prerequisites

  You need to set up a single-node Etcd service discovery cluster. Please follow the instructions in the [Etcd Single-node Installation Guide](https://etcd.io/docs/v3.5/install/) to install and configure the cluster. The installation process includes the following steps:

  - Install Docker and Docker Compose
  - Download the Etcd Docker Compose file
  - Configure the Etcd cluster
  - Start the Etcd cluster using Docker Compose
  - Verify that the Etcd cluster is running
  - Once you have completed the installation and configuration, you can proceed with the example.

* Compilation

We can run the following command to compile the demo.

```shell
bazel build //examples/...
```

*  Run the server program.

We can run the following command to start the server program.

```shell
./bazel-bin/examples/server/helloworld_server --config=examples/server/trpc_cpp_fiber.yaml
```

* Run the client program

We can run the following command to start the client program.

```shell
./bazel-bin/examples/client/client --config=examples/client/trpc_cpp_fiber.yaml
```

* View the etcd data

If you have deployed a single-node Etcd service discovery system following the reference documentation mentioned above, you can log in to the console (etcdctl get /test_trpc --prefix) at 127.0.0.1:2379 to view the backend service status of our example deployment.