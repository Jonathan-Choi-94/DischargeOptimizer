# DischargeOptimizer

댐을 포함하고 있는 강 유역에서 향후 강우 예측량을 받아서 홍수 피해가 최소화되는 방향으로 댐의 방류량을 최적화 하는 소스입니다.
This is the source code for optimizing discharge to minimize a damage from flood at the riverside.

특징:
- HEinsSim 이라는 (주)아인스에스엔씨 소유의 시뮬레이션 엔진을 사용하여 강 유역 핵심 포인트들에 대한 수위을 예측하도록 되어 있습니다.
- EinsOpt 라는 (주)아인스에스엔씨 소유의 모델학습(최적화 Nelder-Mead 알고리즘 사용) 도구를 사용하여 방류량을 최적화하도록 되어 있습니다.
- vcpkg 에서 제공하는 libpqxx 라이브러리를 사용하여 Postgresql 기반 데이터베이스 서버와 통신합니다.
- 지구과학 분야의 과학데이터 형식으로 사용되는 Unidata의 NetCDF 데이터 형식을 읽어오기 위하여 Python의 netCDF4 라이브러리를 사용합니다.
  Python 기반 소스를 c++에서 임베드하여 실행합니다.
  
  
