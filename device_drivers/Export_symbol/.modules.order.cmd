cmd_/home/vlab/git_comp/device_drivers/Export_symbol/modules.order := {   echo /home/vlab/git_comp/device_drivers/Export_symbol/driver1.ko; :; } | awk '!x[$$0]++' - > /home/vlab/git_comp/device_drivers/Export_symbol/modules.order
