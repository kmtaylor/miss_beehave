

cube_raster.v: cube_control.v cube_shifter.v cube_raster_top.v
	@echo \`include \"cypress.v\" > $@
	cat $^ | sed '/`include/d' >> $@ 
	unix2dos $@
