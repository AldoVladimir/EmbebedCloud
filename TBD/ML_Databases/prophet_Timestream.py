#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Oct  8 17:53:06 2022

@author: vladimir
"""

# %% Importar bibliotecas
import awswrangler as wr
import boto3 
#import numpy as np
import pandas as pd
from prophet import Prophet
#import matplotlib.pyplot as plt

# %% Consultar fuentes en Amazon

#Timestream
query = 'SELECT * FROM "IoT_Digitalizacion"."Atmosfera_Albert_CDMX" '\
        'where time between now()-30day and now()'
df = wr.timestream.query(query)

# %% Preprocesamient
#La consulta de timestream está en formato 'wide'
#Hay que cambiarla a formato 'Long'
df = df.pivot(index='time', 
                       columns='measure_name', 
                       values= ['measure_value::double','measure_value::varchar']).dropna(axis = 1, thresh = 10)


#Deshacermos de las columnas multinivel
df.columns = df.columns.get_level_values(1)
df = df.drop(columns="deviceID")

#Formatear columnas con su tipo de dato
df["date-time"] = pd.to_datetime(df["date-time"]) #A fecha
cols=[i for i in df.columns if i not in ["date-time","time"]] #A numerico
for col in cols:
    df[col]=pd.to_numeric(df[col])
    
# %% Previsualizar
variable = "hum_perc"

df.plot(y = variable, x = "date-time" )

df_rolling = df.rolling(120).mean()
df_rolling.plot(y = variable, use_index=True )
# %%Prophet fig
df_prophet = pd.DataFrame()
df_prophet["y"] =df_rolling[variable]
df_prophet["ds"] =df.index.tz_localize(None)


m = Prophet(changepoint_prior_scale=0.01).fit(df_prophet)
# %% Prophet predictions
future = m.make_future_dataframe(periods=24, freq='H')
fcst = m.predict(future)
fig = m.plot(fcst)
