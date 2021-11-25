# DB_pratice

## HyperCloud app

### Install HyperCloud application
Install hyperauth, hypercloud-operator, hypercloud-operator, hypercloud-console
1. Modify k8s.config to suit your environ
    * MAIN_MASTER_IP : Your master node's ip address
      * ex : MAIN_MASTER_IP=172.21.7.7
    * MASTER_NODE_ROOT_PASSWORD : Your master node's root password
      * ex : MASTER_NODE_ROOT_PASSWORD=1234
    * MASTER_NODE_ROOT_USER : Your master node's root user
      * ex : Your master node's root password=root

2. Execute installer script in master node
    ```
    ./hypercloud_app_install.sh
    ```
