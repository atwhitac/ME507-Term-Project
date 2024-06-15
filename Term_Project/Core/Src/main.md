

## Overview

This project aims to document the design and creation process of our ME 507 Term Project: The High Dart-Per-Second Foam Blaster.

![Blaster Assembly](C:\Users\andyr\OneDrive\Documents\GitHub\ME507-Term-Project\Term_Project\Core\Src\photos\side_view.jpg)

The blaster has 2 barrels with two magazines loaded into it, allowing for one or both magazines to be used at once. The blaster also has solenoids designed for a high rate of fire application, and brushless motors controlling flywheel to launch the darts at high speeds.

![Diagram of Blaster Modes](C:\Users\andyr\OneDrive\Documents\GitHub\ME507-Term-Project\Term_Project\Core\Src\photos\modes.png)

The blaster has 12 different firing patterns available, based on both the firing modes and types. The firing modes include the individual left and right magazines, an alternating mode, and a mode where both magazines fire at once. The firing types include single fire, 4-round burst, and fully automatic. The blaster can be in any combination of modes using the user interface on the top of the blaster.


# Mechanical Design 

## Overview

This document details the mechanical design of our high rate of fire  blaster project. The design process involved many 3D-printed parts, CNC machining several key components, soldering components to our board, and assembling the blaster in its entirety.


## CNC Machined Parts

The blaster features several CNC machined components that are crucial for its high rate of fire and durability. Below are images and descriptions of these parts:

### Part 1: CNC Base Plate

![CNC Machined Barrel](C:\Users\andyr\OneDrive\Documents\GitHub\ME507-Term-Project\Term_Project\Core\Src\photos\base_plate.jpg)

The base plate is meant to act as a rigid mounting for a majority of critical components

### Part 2: CNC Machined Trigger Mechanism

![CNC Machined Trigger Mechanism](C:\Users\andyr\OneDrive\Documents\GitHub\ME507-Term-Project\Term_Project\Core\Src\photos\flags.jpg)

The flags are attached to the solenoids, which trigger the beam brake sensors when extended or retracted.

### Part 3: CNC Machined Flywheel Housing

![CNC Machined Flywheel Housing](C:\Users\andyr\OneDrive\Documents\GitHub\ME507-Term-Project\Term_Project\Core\Src\photos\flywheels.jpg)

The flywheels are designed to house the brushless motors inside them. These were CNC machined due to the high speeds of the motors, which requires a higher quality flywheel.


## Mid-Assembly Process

During the assembly process, we integrated each component slowly verifying it works as intended both mechanically and in software.

![Mid-Assembly](C:\Users\andyr\OneDrive\Documents\GitHub\ME507-Term-Project\Term_Project\Core\Src\photos\prototype.jpg)

This photo shows the blaster in its mid-assembly stage, where we were testing solenoids, flywheels, and beam brake sensor functionality.

## Final Product

The final product is a Nerf blaster that has 12 different modes, a safety switch, potentiometers for flywheel speed and solenoid fire rate adjustment, a user interface, and two triggers for both spinning the motors to speed and firing the darts.

![A side view of the final product of the blaster](C:\Users\andyr\OneDrive\Documents\GitHub\ME507-Term-Project\Term_Project\Core\Src\photos\side_view.jpg)

![Another angle of the blaster](C:\Users\andyr\OneDrive\Documents\GitHub\ME507-Term-Project\Term_Project\Core\Src\photos\back_view.jpg)

![Top view showing the LED array](C:\Users\andyr\OneDrive\Documents\GitHub\ME507-Term-Project\Term_Project\Core\Src\photos\top_view.jpg)

The completed blaster, although not as robust as we would like, functions properly with little software issues. There is one issue with the beam brake sensors lifting due to the high vibrations of the solenoids, but a piece of foam inside the top cover solves this issue for the most part.

# Electrical Design

## Overview

The electronics of the blaster are built around custom-designed PCBs, which control the firing mechanism and other functionalities.

![Main Board](C:\Users\andyr\OneDrive\Documents\GitHub\ME507-Term-Project\Term_Project\Core\Src\photos\pcb.jpg)
This first board that was designed was not ultimately used for several issues regarding component failure and a combination of both known and unknown issues. We were able to replace this board with a STM32 BlackPill development board for a functional product.


![Power Board](C:\Users\andyr\OneDrive\Documents\GitHub\ME507-Term-Project\Term_Project\Core\Src\photos\power_board.jpg)
This board was used the 24V Solenoid Driver Board. Power is delivered from the batteries towards the top of the board, and the solenoid wires are connected towards the bottom of the image. The board also distributes 24V to the ESCs. The blue,  green, and yellow wires are the signals used to control the H-bridges. This board worked exceptionally well for the application and allowed us to drive the solenoids at an extremely fast rate of fire. This is done through “Solenoid Clamping” which rapidly decays the current generated by the solenoid when returning using the spring. This allows the solenoid to return much quicker than if the current was just freewheeling through a diode.



## Software Design

The logic for the blaster is written in C++ using the STM32CubeIDE. It has several finite state machines (FSMs) that use a combination of timing, a dart counter, user inputs, and the beam breaks to trigger the transitions that fire the darts. This includes handling the safety, firing modes, potentiometers to control fire rate and motor speed, and the LED array for user input.
[![](https://mermaid.ink/img/pako:eNqNVvFr2zoQ_leEYPthZK2dpt0qtsLy2rwN0vYRZzzYPIJmy7GoIwVbYetK__d30smJ7ORlS8CWTt93-nS-k_REM50LyuiLF5PklsSpagw34lryZc1XqUoVgV8ua5EZqRWZztCSlFqbRWKxDNtAxyGljSCVKAzRRYjDUfubl7Ihbh5ScpVXoiGmFCQX3_VGZcLybN_UcrkU9Y4HWDfQ8EKYR9L8kCYrT3BcwJidGXv4vIWlLRJRoXbW7R4Qewjwf1plUYhaKENWfMl_SSXICtjNnppUfX317fXrK3BudL34V6p8s05V2IPR-eO6M3O3D4BOHIMODPVW6UNeVHxJ3tsP2kUTkNPDRFZl1wsAAecC2f4xBl3YUxhtXOf0ZjJnn5Ob2ZCcOjRMYG2IsS0AfZjuYcCEEGgAYnw__9iHWBtibAtAs09_f9xz5IwIc82jmlL1vFubNfkVTSEjFhGDoNUmsMSMTKAUXCc02zmQMcecJTMIEW9Emio3gBO4UO_ct6yYdTAxOW1pRmYPYMk2de3abbif_ZdBilumFz6Ty7KnHE2tdNfrDNhAelZPvROCY70FeMKWGrMuLA6oxxeBjCBH_UISV-MMXwScYQE1UKeF7SW6EkrLvCHXsuHfK5G3mxVSQNp9UXj-InFbBY6D2elupFoy0i75n1o0jcidbpRDJm0JbS2JXx6S21DYtp3u7qi3lnt_52XcAWfCq8rK-L2KaN-TJ6M73_HLPuivLwQCGaa_LaqnNohjbcpuGjlLm0W2E5ptxSIjDvmtOWbu9UcZ3tEEm4HPB-zLwjt59y4rtczE1ZU_HgCJ-ePLiRS6_sHrvLErd4lIxjx7QFOfMmQeEpKclx4H97hWBdq2muzuGsogU97AuQjxyl3O2Anch-jtIUfdDHtunKpTsnUT1HP4DHQEgtlcrgS5cszFzE720joaC74i45o_wOGWCO--M2vc9zo87pX8mdvokNhd74xdQ_ot_oJbgYFkfg9n7c9DQn7H6KRUePKSg8l1tpddIdOVTfjBzuzxvJaqX3Q2BC_bu4o-xNo6ZDcVnBWhyu4FoHPO-h0Tir2Cu5d7hQPjTd0Y5p6h-cPGaGYffpN0PFufDm4PR6xKNzMEbuQDDQy7PpzNw0iAa-vdurGfwk5zAHYeRdt9nw7oStQrLnO4d7qlpRTuVCuRUgbNnNcPKUUcB3fJo8ooM_VGDGitN8uSsoJDvAZ0s853d9UWsuaKsif6k7L44vzkzeXo_OLt5SganZ1fDugjZdHzgP7SGvDxSYS_4cXoIh4OL98MqMglfJNbvBO7q7Hz-MUR7ATP_wFZ5qKT?type=png)](https://mermaid.live/edit#pako:eNqNVvFr2zoQ_leEYPthZK2dpt0qtsLy2rwN0vYRZzzYPIJmy7GoIwVbYetK__d30smJ7ORlS8CWTt93-nS-k_REM50LyuiLF5PklsSpagw34lryZc1XqUoVgV8ua5EZqRWZztCSlFqbRWKxDNtAxyGljSCVKAzRRYjDUfubl7Ihbh5ScpVXoiGmFCQX3_VGZcLybN_UcrkU9Y4HWDfQ8EKYR9L8kCYrT3BcwJidGXv4vIWlLRJRoXbW7R4Qewjwf1plUYhaKENWfMl_SSXICtjNnppUfX317fXrK3BudL34V6p8s05V2IPR-eO6M3O3D4BOHIMODPVW6UNeVHxJ3tsP2kUTkNPDRFZl1wsAAecC2f4xBl3YUxhtXOf0ZjJnn5Ob2ZCcOjRMYG2IsS0AfZjuYcCEEGgAYnw__9iHWBtibAtAs09_f9xz5IwIc82jmlL1vFubNfkVTSEjFhGDoNUmsMSMTKAUXCc02zmQMcecJTMIEW9Emio3gBO4UO_ct6yYdTAxOW1pRmYPYMk2de3abbif_ZdBilumFz6Ty7KnHE2tdNfrDNhAelZPvROCY70FeMKWGrMuLA6oxxeBjCBH_UISV-MMXwScYQE1UKeF7SW6EkrLvCHXsuHfK5G3mxVSQNp9UXj-InFbBY6D2elupFoy0i75n1o0jcidbpRDJm0JbS2JXx6S21DYtp3u7qi3lnt_52XcAWfCq8rK-L2KaN-TJ6M73_HLPuivLwQCGaa_LaqnNohjbcpuGjlLm0W2E5ptxSIjDvmtOWbu9UcZ3tEEm4HPB-zLwjt59y4rtczE1ZU_HgCJ-ePLiRS6_sHrvLErd4lIxjx7QFOfMmQeEpKclx4H97hWBdq2muzuGsogU97AuQjxyl3O2Anch-jtIUfdDHtunKpTsnUT1HP4DHQEgtlcrgS5cszFzE720joaC74i45o_wOGWCO--M2vc9zo87pX8mdvokNhd74xdQ_ot_oJbgYFkfg9n7c9DQn7H6KRUePKSg8l1tpddIdOVTfjBzuzxvJaqX3Q2BC_bu4o-xNo6ZDcVnBWhyu4FoHPO-h0Tir2Cu5d7hQPjTd0Y5p6h-cPGaGYffpN0PFufDm4PR6xKNzMEbuQDDQy7PpzNw0iAa-vdurGfwk5zAHYeRdt9nw7oStQrLnO4d7qlpRTuVCuRUgbNnNcPKUUcB3fJo8ooM_VGDGitN8uSsoJDvAZ0s853d9UWsuaKsif6k7L44vzkzeXo_OLt5SganZ1fDugjZdHzgP7SGvDxSYS_4cXoIh4OL98MqMglfJNbvBO7q7Hz-MUR7ATP_wFZ5qKT)
