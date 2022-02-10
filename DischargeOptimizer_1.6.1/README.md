DischargeOptimizer
==================
>강 유역 강우 예측량 기반, 홍수 피해 최소화 방향으로 댐의 방류량을 최적화하는 모듈

>The module for optimizing discharge at dam to minimize a damage from flood at the riverside.

특징 (Specification)
-------------------
- HEinsSim((주)아인스에스엔씨 소유) 시뮬레이션 엔진 사용, 강 유역 핵심 지점들에 대한 수위을 예측.
- EinsOpt((주)아인스에스엔씨 소유) 모델학습 도구(최적화 Nelder-Mead 알고리즘 기반) 사용, 방류량 최적화.
- libpqxx(vcpkg) 라이브러리 사용, Postgresql 기반 데이터베이스 서버와 통신.
- netCDF4(Python) 사용, Unidata의 NetCDF 데이터 형식을 읽어옴.
	- NetCDF 데이터 형식은 지구과학 분야에서 과학데이터 형식으로 사용됨.
	- Python 소스를 c++에서 임베드하여 실행.

- This uses HEinsSim(Simulation engine from EinsS&C Co.,Ltd), to predict a height at important points of a riverside.
- This uses EinsOpt(Model training tool which uses Nelder-Mead optimizing algorithm from EinsS&C Co.,Ltd), 
to optimize discharge.
- This uses libpqxx library in vcpkg, to communicate with a database server based on Postgresql.
- This uses netCDF4 library in Python, to read NetCDF data form of Unidata.
	- NetCDF data form is often used in geoscience field as a science data form.
	- This runs Python based source in c++ by embedding python.