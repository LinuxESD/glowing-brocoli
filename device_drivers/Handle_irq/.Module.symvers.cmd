cmd_/home/vlab/git_comp/device_drivers/Handle_irq/Module.symvers := sed 's/\.ko$$/\.o/' /home/vlab/git_comp/device_drivers/Handle_irq/modules.order | scripts/mod/modpost -m -a  -o /home/vlab/git_comp/device_drivers/Handle_irq/Module.symvers -e -i Module.symvers   -T -
