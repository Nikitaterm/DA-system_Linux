export KERNELDIR = /home/nikita/Cubieboard2/linux-sunxi/

make: build_driver
	@echo "*****Build completed successfully!*****"

clean: clean_driver clean_out

clean_out:
	rm -rf out/*

build_driver:
	@echo "Building driver..."
	cd driver && $(MAKE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
	@echo "Moving into out/..."
	if [ ! -d "out/driver" ]; then \
	    mkdir out/driver; \
	fi
	cp driver/*.ko out/driver/
	cp driver/DA_driver.sh out/driver/

clean_driver:
	@echo "Cleaning..."
	cd driver && $(MAKE) clean
