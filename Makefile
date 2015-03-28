export KERNELDIR = /lib/modules/3.4.43/build

make: build_driver
	@echo "*****Build completed successfully!*****"

clean: clean_driver clean_out

clean_out:
	rm -rf out/*

build_driver:
	@echo "Building driver..."
	cd driver && $(MAKE)
	@echo "Moving into out/..."
	mv driver/*.ko out/

clean_driver:
	@echo "Cleaning..."
	cd driver && $(MAKE) clean

