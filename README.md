Airline Scheduling
==================

Proyecto de Algoritmia
Enero 2015

Bernat Moix Alcaraz - bernat.moix@est.fib.upc.edu
Jordi Vilaseca Corderroure - jordi.vilaseca.corderroure@est.fib.upc.edu

# Compilar:
	
	make


# Limpiar el directorio (binarios):
	
	make clean


# Ejecutar:
	
	./airlines <numero de aeropuertos/ciudades> <version> <algoritmo>

	## version:
		1 = version 1, un unico piloto por trayecto
		2 = version 2, los pilotos pueden ir como pasajeros

	## algoritmo:
		1 = Edmonds-Karp
		2 = Dinic

# Archivos adjuntos:

	- Informe.pdf: Informe que explica y analiza el desarrollo del proyecto
	- airlines.cc: Codigo fuente
	- Makefile: Compilacion
	- Resultado1.txt: Resultados obtenidos en la version 1
	- Resultado2.txt: Resultados obtenidos en la version 2
	- Directorio 'Times': Datos experimentales de los tiempos de ejecucion
