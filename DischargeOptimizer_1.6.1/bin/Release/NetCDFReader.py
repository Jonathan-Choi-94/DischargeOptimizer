from netCDF4 import Dataset
import numpy as np
import json

def Q_parser(File_name):
    from netCDF4 import Dataset
    import numpy as np
    import json
    
    test = Dataset(File_name)
    K_RIVER = test.groups['K_RIVER']

    # Parsing Q(flow rate) at each station 
    Q = np.transpose(K_RIVER.groups['Output'].groups['Station'].variables['Q'][:])
    Q_list = Q.tolist()

    return Q_list


def WL_parser(File_name):
    from netCDF4 import Dataset
    import numpy as np
    import json
    
    test = Dataset(File_name)
    K_RIVER = test.groups['K_RIVER']
    
    # Parsing WL(Water Level) at each station 
    WL = np.transpose(K_RIVER.groups['Output'].groups['Station'].variables['WL'][:])
    WL_list = WL.tolist()

    return WL_list

