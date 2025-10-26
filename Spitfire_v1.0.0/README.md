# Spitfire Cockpit Control - v1.1.4
This configuration of the Control requires two Howstrong Simulations Mega shields, powered by a 12V, 80W Mean Well power supply. There is a daisy chain power connection between them supplying power to the second shield.


## Pinout
Connect the instruments to the connectors labelled below. 

### Mega Shield A
The first Mega Shield should be connected with the cockpit instruments as labelled below. It should have the `.ino` file called `MegaShield_v1.1.4A.ino` uploaded to it.

#### Mega Shield A - Connectors J-Ax

| Connector| Instrument | Arduino Step/Direction Pins |
| --- | --- | --- |
| J-A1 | Voltmeter | 9, 8|
| J-A2 | Tachometer | 6, 7 |
| J-A3 | Boost Pressure | 11, 10 |
| J-A4 | Radiator Temperature| 13, 12 |
| J-A5 | Oil Temperature | 14, 15 |
| J-A6 | Oil Pressure | 17, 16 |
| J-A7 | Fuel Level | 4, 5 |
| J-A8 | Nose Trim | 2, 3 |
| J-A9 | Turn Coordinator| 22, 23 |
| J-A10| Slip Coordinator| 24, 25 |
| J-A11| Variometer (RoC) | 19, 18 |
| J-A12| | 20, 21 |

#### Mega Shield A - Connectors J-Bx

| Connector | Instrument | Arduino Pins |
| --- | --- | --- |
| J-B1 | DI Encoder | A5, A4 |
| J-B2 | | A3, A2 |
| J-B3 | Undercarriage Indicator | A1, A0 |

#### Mega Shield A - Connectors J-Cx

| Connector | Instrument | Arduino Pins |
| --- | --- | --- |
| J-C1 | DI IR Sensor | 26 |
| J-C2 | ASI IR Sensor | 28 |
| J-C3 | Gunsight Switch | 27 |
| J-C4 | Navigation Lights Switch | 29 |
| J-C5 | Fuel Gauge Switch | 31 |
| J-C6 | Supercharger Mode Select Switch| 30 |
| J-C7 | Supercharger Lamp | 32 |
| J-C8 | Low Fuel Pressure Lamp | 33 |
| J-C9 | Magnetos 1 Switch | 35 |
| J-C10 | Magneto 2 Switch| 37 |
| J-C11 | | 34 |
| J-C12 | | 36 |
| J-C13 | | 38 |
| J-C14 | | 39 |
| J-C15 | | 41 |
| J-C16 | | 40 |
| J-C17 | | 43 |
| J-C18 | | 42 |

#### Mega Shield A - Connectors J-Dx
Connectors D are 2 pin connectors which provide 12V only.

| Connector | Instrument |
| --- | --- |
| J-D1 | DI Motor Power |
| J-D2 | ASI Motor Power |
|  |  |
|  |  |
|  |  |
|  |  |

#### Mega Shield A  - Connectors J-Ex

| Connector | Instrument | Arduino Pins |
| --- | --- | --- |
| J-E1 | DI Motor Driver | A8, A9 |
| J-E2 | ASI Motor Drivers | A12, A13 |
| J-E3 |  | 52, 53 |
| J-E4 |  | 48, 49 |
| J-E5 |  | 44, 45 |
| J-E6 |  | A6, A7 |
| J-E7 |  | A10, A11 |
| J-E8 |  | A14, A15 |
| J-E9 |  | 50, 51 |
| J-E10 |  | 49, 47 |


