cmd_/home/vlab/git_comp/device_drivers/File_op_trila/modules.order := {   echo /home/vlab/git_comp/device_drivers/File_op_trila/driver.ko;   echo /home/vlab/git_comp/device_drivers/File_op_trila/check_driver.ko; :; } | awk '!x[$$0]++' - > /home/vlab/git_comp/device_drivers/File_op_trila/modules.order
