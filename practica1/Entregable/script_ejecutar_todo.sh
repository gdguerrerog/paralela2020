#!/bin/bash

echo Bienvenido al programa de difuminado de imagenes:
cont="continue"
skernel=20
first=-1;
timevars=""
while [ $cont == "continue" ]; do
	skernel=20
	echo Ingrese el nombre de la imagen a difuminar
	read input
	
	echo Ingrese el nombre de la imagen nueva
	read output

	while [ $skernel -gt 15 ] || [ $skernel -lt 3 ]; do
		echo Ingrese el tamaño del kernel \(Entre 3 y 15\)
		read skernel
	done	

	echo Ingrese el numero de hilos
	read nhilos

	
	
	gcc blur-effect.c -o blur_effect -lm -fopenmp
	timevars=time ./blur-effect "$input" "$output" "$skernel" "$nhilos"
	
	if [ $first -lt 0 ]; then
		echo Imagen $input con tamaño de Kernel $skernel y numero de hilos $nhilos con resultado en archivo $output tiempos: >results.txt
	else
		echo Imagen $input con tamaño de Kernel $skernel y numero de hilos $nhilos con resultado en archivo $output tiempos: >>results.txt
	fi	
	
	echo $timevars;
	
	first=1;
	echo Si desea ingresar mas imagenes escriba \"continue\" de lo contrario presione enter	
	read cont
	

done

