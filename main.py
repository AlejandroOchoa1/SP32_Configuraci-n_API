from fastapi import FastAPI   #framework para crear API
from fastapi.middleware.cors import CORSMiddleware #permite que frontend pueda consumir la API
from pydantic import BaseModel #define la estructura y validacion de datos
from collections import deque  #cola eficiente, permiite limitar cantidad de datos guardados
from datetime import datetime, timezone #hora 

app = FastAPI() # se crea la api

#origen exacto 
origins = [
    "http://127.0.0.1:5500",
    "http://localhost:5500",
    "http://10.117.231.66:5500",
]
# configuracion de cors que es lo que permite el consumo de la api para el frontend
app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,     
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Buffer en memoria de los últimos 100 registros
HIST = deque(maxlen=100)

#modelaje de datos
class DatosSensor(BaseModel):
    temperatura: float
    humedad: float

#ENDPOINT para recibir datos
@app.post("/api/datos")
async def recibir_datos(data: DatosSensor):
    item = {
        "ts": datetime.now(timezone.utc).isoformat(),
        "temperatura": data.temperatura,
        "humedad": data.humedad
    } 
    HIST.append(item)
    return {"status": "ok"}

#endpoint obtiene el ultimo DATO
@app.get("/api/ultimo")
async def ultimo():
    if not HIST:
        return {"ts": None, "temperatura": None, "humedad": None}
    return HIST[-1]  # sino encuentra datos devuelve NONE


#ENDPOINT get obtiene el historial
@app.get("/api/historial")
async def historial(limit: int = 10): #por defecto 10 datos
    # devolver los más recientes primero
    items = list(HIST)[-limit:][::-1]
    return {"items": items} #devuelve los datos en un json