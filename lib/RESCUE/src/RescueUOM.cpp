/********************************************************************

  RescueUOM.cpp

  Table-like access to UOM conversion tables.

  Rod Hanks,  Nov 15 2007

*********************************************************************/
#include "RescueUOM.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
RescueUOM::UOM RescueUOM::uom[] = {
    {"inverse henry",
     {"reluctance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per Kelvin",
     {"volumetric thermal expansion", "linear thermal expansion", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per kilogram", {"per mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per metre",
     {"per length", "wave number", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per square metre",
     {"per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per cubic metre",
     {"per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per Newton", {"per force", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per Pascal",
     {"bulk compressibility", "compressibility", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per second",
     {"per time", "operations per time", "frequency", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per Volt",
     {"per electric potential", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"ampere",
     {"electric current", "magnetic potential difference", "magnetomotive force", "", "", "", "", "", "", "", "", "",
      ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"amperes metres squared",
     {"electromagnetic moment", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"amperes/metre",
     {"linear electric current density", "magnetic field strength", "magnetization", "", "", "", "", "", "", "", "", "",
      ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"amperes/square metre",
     {"current density", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"bel",
     {"level of power intensity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"bels/metre",
     {"attenuation per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"bels/octave",
     {"attenuation", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"baud",
     {"data transmission speed", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"bits per second",
     {"data transmission speed", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"becquerel",
     {"activity (of radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"becquerel per kilogram",
     {"specific activity (of radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"byte", {"digital storage", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"coulomb",
     {"electric capacity", "electric charge", "electric flux", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"coulomb metres",
     {"electric dipole moment", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"coulomb per kilogram",
     {"exposure (radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"coulombs/square metre",
     {"electric polarization", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"coulombs/cubic metre",
     {"charge density", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"candela",
     {"luminous intensity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"candelas/square metre",
     {"luminance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"equivalent",
     {"electrochemical equivalent", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"equivalent per kilogram",
     {"equivalent per mass", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"equivalents per cubic metre",
     {"equivalent per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"euclid",
     {"dimensionless", "volumic concentration", "mass concentration", "linear strain", "shear strain", "attenuation",
      "poisson ratio", "", "", "", "", "", ""},
     7,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"farad", {"capacitance", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"farads/metre",
     {"permittivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"gas unit",
     {"volumic concentration", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"API gamma ray units",
     {"gamma ray API unit", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"gray", {"absorbed dose", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"henry",
     {"self inductance", "permeance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"henries/metre",
     {"magnetic permeability", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"hertz", {"frequency", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"joule",
     {"moment of force", "moment of couple", "torque", "energy", "work", "", "", "", "", "", "", "", ""},
     5,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"joules per delta kelvin",
     {"heat capacity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"joules/kilogram",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"joules/kilogram degree kelvin",
     {"specific heat capacity", "massic heat capacity", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"joules/cubic metre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"joules/mole",
     {"molar thermodynamic energy", "chemical potential", "affinity of a chemical reaction", "", "", "", "", "", "", "",
      "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"joules/mole degree kelvin",
     {"molar heat capacity", "molar entropy", "molar gas constant", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kelvin",
     {"thermodynamic temperature", "delta temperature", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kelvin metres squared/watt",
     {"thermal insulance", "coefficient of thermal insulation", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees kelvin/metre",
     {"temperature per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kelvin per second",
     {"temperature per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"delta kelvin per watt",
     {"thermal resistance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilogram", {"mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"meter-kilogram",
     {"mass length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilogram metres/second",
     {"momentum", "impulse", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilogram metres squared",
     {"moment of inertia", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms/joule",
     {"mass per energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms/metre",
     {"linear density", "linear mass", "mass per length", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms/square metre",
     {"surface density", "areic mass", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms/square metre seconds",
     {"mass per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms/cubic metre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilogram/metre fourth",
     {"mass per volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms/second",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"lumen", {"luminous flux", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"lumen second",
     {"quantity of light", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"lumens/watt",
     {"luminous efficacy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"lux", {"illuminance", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"lux seconds",
     {"light exposure", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"metre",
     {"length", "breadth", "height", "depth", "thickness", "radius", "radius of curvature", "Cartesian coordinates",
      "diameter", "length of path", "distance", "wavelength", "mean free path"},
     13,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"metres/degree kelvin",
     {"length per temperature", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"metres/second", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"metres/second squared",
     {"acceleration linear", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square metres", {"area", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"square metres/kilogram",
     {"mass attenuation coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square metres/mol",
     {"cross section absorption", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square metres/second Pascal",
     {"unit productivity index", "mobility", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square metres/second",
     {"kinematic viscosity", "thermal diffusivity", "diffusion coefficient", "area per time", "", "", "", "", "", "",
      "", "", ""},
     4,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres", {"volume", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"cubic metres/joule",
     {"isothermal compressibility", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/kilogram",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/mole",
     {"molar volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/second pascal",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"(cubic metres/second) per pascal",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/pascal second squared",
     {"specific productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/second",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/seconds squared",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/std cubic metres, 0 deg C",
     {"volume per standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/std cubic metres, 15 deg C",
     {"volume per standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"metres fourth",
     {"second moment of area", "moment of section", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"metres fourth/second",
     {"volume length per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"mole",
     {"amount of substance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles/square metre",
     {"mole per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles/square metre second",
     {"mole per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles/cubic metre",
     {"concentration of B", "amount of a substance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles/second",
     {"mole per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"newton", {"force", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"newton square metres",
     {"force area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"newtons/metre",
     {"force per length", "energy per area", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"newtons/cubic metre",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"newtons fourth metres/kilogram",
     {"parachor", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"API neutron units",
     {"neutron API unit", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"octave",
     {"frequency interval", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"ohm",
     {"resistance", "impedance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"ohm metre",
     {"electrical resistivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"ohm per metre",
     {"resistivity per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pascal",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pascal seconds",
     {"dynamic viscosity", "mass per time per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pascal seconds/cubic metre",
     {"pressure time per volume", "acoustic impedance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pascal second /cubic metre squared",
     {"nonDarcy flow coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pascals/metre",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pascals/cubic metre",
     {"Darcy flow coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pascal/ second",
     {"pressure per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pascal squared",
     {"pressure squared", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pH", {"pH", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"radian", {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"radians/metre",
     {"angle per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"radians per cubic metre",
     {"angle per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"radians/second",
     {"angular velocity", "rotational frequency", "circular frequency", "angle per time", "rotational velocity", "", "",
      "", "", "", "", "", ""},
     5,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"radians/second squared",
     {"angular acceleration", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"siemens",
     {"electric conductance", "admittance", "susceptance", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"second", {"time", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"siemens/metre",
     {"conductivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"seconds/metre",
     {"time per length", "slowness", "interval transit time", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"seconds/cubic metre",
     {"time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"standard cubic metres at 0 deg Celsius",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"std cubic metres, 0 deg C/square metre",
     {"standard volume  per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"std cubic metres, 0 deg C/cubic metre",
     {"standard volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"standard cubic metres at 15 deg Celsius",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"std cubic metres, 15 deg C/square metre",
     {"standard volume  per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"std cubic metres, 15 deg C/cubic metre",
     {"stadard volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"std cubic metres, 15 deg C/second",
     {"standard volume per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"steradian", {"solid angle", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"sievert",
     {"dose equivalent", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"sievert per second",
     {"dose equivalent rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"tesla",
     {"magnetic induction", "magnetic flux density", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"volt",
     {"electric potential", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"volts/Bel",
     {"potential difference per per power drop", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"volts/metre",
     {"electric field strength", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"watt",
     {"power", "heat flow rate", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Watts per delta kelvin",
     {"thermal conductance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"watts/metre kelvin",
     {"energy length per time area temperature", "thermal conductivity", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"watts/square metre",
     {"density of heat flow rate", "poynting vector", "radiant energy", "fluence rate", "radiant exitance",
      "irradiance", "sound intensity", "energy fluence rate", "", "", "", "", ""},
     8,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"watts/square metre kelvin",
     {"heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"watts/square metre steradian",
     {"radiance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"watts/cubic metre",
     {"power per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"watts/cubic metre kelvin",
     {"volumetric heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"watts/steradian",
     {"radiant intensity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"weber", {"magnetic flux", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"weber metres",
     {"magnetic dipole moment", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"webers/metre",
     {"magnetic vector potential", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"percent",
     {"dimensionless", "area per area", "volumic concentration", "mass concentration", "force per force",
      "linear strain", "relative elongation", "relative power", "length per length", "", "", "", ""},
     9,
     32,
     RescueUOM::FACTOR,
     0,
     0.01,
     1,
     0},
    {"per annum", {"per time", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per angstrom",
     {"per length", "wave number", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per bar",
     {"bulk compressibility", "compressibility", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per barrel", {"per volume", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per centimetre",
     {"per length", "wave number", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per day", {"per time", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per degree Celsius",
     {"volumetric thermal expansion", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per degree Fahrenheit",
     {"volumetric thermal expansion", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per degree Rankine",
     {"volumetric thermal expansion", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per foot", {"per length", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per square foot",
     {"per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per cubic foot",
     {"per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per gram", {"per mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per UK gallon",
     {"per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per US gallon",
     {"per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per hour", {"per time", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per inch", {"per length", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per square kilometre",
     {"per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per kilopascal",
     {"bulk compressibility", "compressibility", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per litre", {"per volume", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per pound force",
     {"per force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per pound", {"per mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per mile", {"per length", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per square mile",
     {"per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per minute", {"per time", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per millimetre",
     {"per length", "wave number", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per nanometre",
     {"per length", "wave number", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per pico pascal",
     {"bulk compressibility", "compressibility", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per pounds/square inch",
     {"bulk compressibility", "compressibility", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per micro pounds per square inch",
     {"bulk compressibility", "compressibility", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per microvolt",
     {"per electric potential", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per week", {"per time", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"per yard", {"per length", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"thousand cubic feet",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     28.3168,
     1,
     0},
    {"thousand cubic feet per barrel",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     28.3168,
     0.158987,
     0},
    {"thousand cubic feet per day",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand cubic feet per day per foot",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand cubic feet per day per psi",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand cubic metres per day",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand cubic meter per day per meter",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand cubic metres per hour",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand cubic meters per hour per meter",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand (cubic meter per day)-meter",
     {"volume length per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"100000 years",
     {"time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     121,
     RescueUOM::FACTOR,
     0,
     3.15582e+012,
     1,
     0},
    {"ten thousand kilograms per cubic metre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"annum",
     {"time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     121,
     RescueUOM::FACTOR,
     0,
     3.15582e+007,
     1,
     0},
    {"Ampere hour",
     {"electric capacity", "electric charge", "electric flux", "", "", "", "", "", "", "", "", "", ""},
     3,
     22,
     RescueUOM::FACTOR,
     0,
     3600,
     1,
     0},
    {"ampere per square centimeter",
     {"current density", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"ampere per square foot",
     {"current density", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Ampere/millimetre",
     {"linear electric current density", "magnetic field strength", "magnetization", "", "", "", "", "", "", "", "", "",
      ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Ampere/square millimetre",
     {"current density", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"acre", {"area", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 73, RescueUOM::FACTOR, 0, 4046.87, 1, 0},
    {"acre foot",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     1233.49,
     1,
     0},
    {"acre feet/million stbs, 60 deg F",
     {"volume per standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"attogram", {"mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 53, RescueUOM::FACTOR, 0, 1e-021, 1, 0},
    {"attojoule",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1e-018,
     1,
     0},
    {"Angstrom", {"length", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 69, RescueUOM::FACTOR, 0, 1e-010, 1, 0},
    {"Technical atmosphere",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     98066.5,
     1,
     0},
    {"Atmosphere",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     101325,
     1,
     0},
    {"Atmospheres per ft",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"atmosphere per hour",
     {"pressure per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Atmospheres per hundred metre",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Atmospheres/metre",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barn", {"area", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 73, RescueUOM::FACTOR, 0, 1e+028, 1, 0},
    {"barns/cubic centimetre",
     {"area per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barns/electron",
     {"cross section absorption", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"bar",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     100000,
     1,
     0},
    {"bar per hour",
     {"pressure per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"bar per kilometer",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"bar per meter",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"bar squared",
     {"pressure squared", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     113,
     RescueUOM::FACTOR,
     0,
     1e+010,
     1,
     0},
    {"bar squared per centipoise",
     {"pressure squared per (dynamic viscosity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrel", {"volume", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 78, RescueUOM::FACTOR, 0, 0.158987, 1, 0},
    {"barrel per hundred barrel",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.01,
     1,
     0},
    {"barrels/acre",
     {"volume per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     69,
     RescueUOM::FRACTION,
     0,
     0.158987,
     4046.88,
     0},
    {"barrel/acre foot",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     0.158987,
     1233.49,
     0},
    {"barrel/barrel",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"barrels/centiPoise day psi",
     {"specific productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrel/day",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrels/day acre foot",
     {"volume per time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrels/day foot",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrels/day foot pounds/sq in",
     {"unit productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrel/day pounds/square inch",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrels/day per day",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrel/foot",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FRACTION,
     0,
     0.158987,
     0.3048,
     0},
    {"barrel per cubic foot",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     0.158987,
     0.0283169,
     0},
    {"barrel/hour",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrels/hour/hour",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrel/inch",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FRACTION,
     0,
     0.158987,
     0.0254,
     0},
    {"barrel per thousand cubic feet",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     0.158987,
     28.3168,
     0},
    {"barrel per day per kilopascal",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrel per million cubic feet",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     0.158987,
     28316.8,
     0},
    {"barrel/mile",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FRACTION,
     0,
     0.158987,
     1609.34,
     0},
    {"barrel per minute",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrels/million std cubic feet, 60 degF",
     {"volume per stancard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrel per day per psi",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrels/stock tank barrel, 60 deg F",
     {"volume per standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrel per U.K. ton",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"barrel per U.S. ton",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"billion cubic feet",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     2.83169e+007,
     1,
     0},
    {"bit",
     {"digital storage", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     21,
     RescueUOM::FRACTION,
     0,
     1,
     8,
     0},
    {"British thermal unit",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1055.06,
     1,
     0},
    {"Btus/hour foot squared deg F per inch",
     {"energy length per time area temperature", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"million Btus/hour",
     {"heat flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     293071,
     1,
     0},
    {"British thermal units/barrel",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Btus/brake-horsepower hour",
     {"relative power", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.000393015,
     1,
     0},
    {"British thermal units/cubic foot",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"British thermal units/U.K. gallon",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"British thermal units/U.S. gallon",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"British thermal unit/hour",
     {"heat flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     0.293071,
     1,
     0},
    {"British thermal units/hour foot deg F",
     {"thermal conductivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Btus/hour per square foot",
     {"density of heat flow rate", "poynting vector", "radiant energy", "fluence rate", "radiant exitance",
      "irradiance", "sound intensity", "energy fluence rate", "", "", "", "", ""},
     8,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Btus/hour foot squared deg F",
     {"heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Btus/hour foot squared deg R",
     {"heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"British thermal units/hour cubic foot",
     {"power per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Btus/hour foot cubed deg F",
     {"volumetric heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Btus/hour metre squared deg C",
     {"heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"British thermal units/pound mass",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"British thermal units/pound mass deg F",
     {"specific heat capacity", "massic heat capacity", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"British thermal units/pound mass deg R",
     {"specific heat capacity", "massic heat capacity", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"British thermal units/minute",
     {"heat flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     17.5843,
     1,
     0},
    {"British thermal units/pound mass mol",
     {"molar thermodynamic energy", "chemical potential", "affinity of a chemical reaction", "", "", "", "", "", "", "",
      "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Btus/pound mass mol deg F",
     {"molar heat capacity", "molar entropy", "molar gas constant", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"British thermal units/second",
     {"heat flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     1055.06,
     1,
     0},
    {"British thermal units/second square foot",
     {"density of heat flow rate", "poynting vector", "radiant energy", "fluence rate", "radiant exitance",
      "irradiance", "sound intensity", "energy fluence rate", "", "", "", "", ""},
     8,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Btus/second per square foot deg F",
     {"heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Btus/second per cubic foot",
     {"power per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Btus/second per cubic foot deg F",
     {"volumetric heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cycle",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FACTOR,
     0,
     6.28319,
     1,
     0},
    {"Coulombs/square centimetre",
     {"electric polarization", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Coulombs/cubic centimeter",
     {"charge density", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"coulomb per gram",
     {"exposure (radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Coulombs/square millimetre",
     {"electric polarization", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"Coulombs/cubic millimetre",
     {"charge density", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cycles/second",
     {"angular velocity", "frequency", "rotational frequency", "circular frequency", "angle per time",
      "rotational velocity", "", "", "", "", "", "", ""},
     6,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calorie",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     4.184,
     1,
     0},
    {"calories/cubic centimetre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/gram",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/gram degree Kelvin",
     {"specific heat capacity", "massic heat capacity", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/hour centimetre degree Celsius",
     {"thermal conductivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/hour centimetre squared",
     {"density of heat flow rate", "poynting vector", "radiant energy", "fluence rate", "radiant exitance",
      "irradiance", "sound intensity", "energy fluence rate", "", "", "", "", ""},
     8,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/hour square centimetre deg C",
     {"heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/hour cubic centimetre",
     {"power per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/kilogram",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/pound mass",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/milliliter",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/cubic millimetre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/gram mol degree celsius",
     {"molar heat capacity", "molar entropy", "molar gas constant", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/second centimetre deg C",
     {"thermal conductivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/second square centimetre deg C",
     {"heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"calories/second cubic centimetre",
     {"power per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"centesimal second",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FRACTION,
     0,
     3.14159,
     2e+006,
     0},
    {"centiEuclid",
     {"dimensionless", "poisson ratio", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     0.01,
     1,
     0},
    {"centesimal minute",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FRACTION,
     0,
     3.14159,
     20000,
     0},
    {"ch", {"power", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 139, RescueUOM::FACTOR, 0, 735.499, 1, 0},
    {"ch hours",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     2.6478e+006,
     1,
     0},
    {"Benoit chain (1895 A)",
     {"length", "breadth", "height", "thickness", "radius", "Cartesian coordinates", "diameter", "distance", "", "", "",
      "", ""},
     8,
     69,
     RescueUOM::FACTOR,
     0,
     20.1168,
     1,
     0},
    {"Benoit chain (1895 B)",
     {"length", "breadth", "height", "thickness", "radius", "Cartesian coordinates", "diameter", "distance", "", "", "",
      "", ""},
     8,
     69,
     RescueUOM::FRACTION,
     0,
     792,
     39.3701,
     0},
    {"Clarke chain",
     {"length", "breadth", "height", "thickness", "radius", "Cartesian coordinates", "diameter", "distance", "", "", "",
      "", ""},
     8,
     69,
     RescueUOM::FACTOR,
     0,
     20.1166,
     1,
     0},
    {"Sears chain",
     {"length", "breadth", "height", "thickness", "radius", "Cartesian coordinates", "diameter", "distance", "", "", "",
      "", ""},
     8,
     69,
     RescueUOM::FRACTION,
     0,
     792,
     39.3701,
     0},
    {"chus", {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""}, 2, 41, RescueUOM::FACTOR, 0, 1899.1, 1, 0},
    {"US Survey chain",
     {"length", "breadth", "height", "thickness", "radius", "Cartesian coordinates", "diameter", "distance", "", "", "",
      "", ""},
     8,
     69,
     RescueUOM::FRACTION,
     0,
     792,
     39.37,
     0},
    {"curie",
     {"activity (of radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     19,
     RescueUOM::FACTOR,
     0,
     3.7e+010,
     1,
     0},
    {"centimetre",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "Cartesian coordinates", "diameter",
      "length of path", "distance", "wavelength", "mean free path", ""},
     12,
     69,
     RescueUOM::FACTOR,
     0,
     0.01,
     1,
     0},
    {"centimeter per year",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"centimetre/second",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"centimetre/second squared",
     {"acceleration linear", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square centimetre",
     {"area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     0.0001,
     1,
     0},
    {"centimetres squared/gram",
     {"mass attenuation coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"centimetres squared/second",
     {"kinematic viscosity", "thermal diffusivity", "area per time", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic centimetre",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"cubic centimeter per thirty minutes",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic centimeters/ cubic centimetres",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"cubic centimetres/gram",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic centimeter per hour",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic centimetre/cubic metre",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"cubic centimeter per minute",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic centimeter per second",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"centimetres fourth",
     {"second moment of area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     89,
     RescueUOM::FACTOR,
     0,
     1e-008,
     1,
     0},
    {"cm of water at 4 degC.",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     98.0638,
     1,
     0},
    {"centipoise",
     {"dynamic viscosity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"ten milli second",
     {"time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     121,
     RescueUOM::FACTOR,
     0,
     0.01,
     1,
     0},
    {"centiStoke",
     {"kinematic viscosity", "area per time", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"carat", {"mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 53, RescueUOM::FACTOR, 0, 0.0002, 1, 0},
    {"capture unit",
     {"area per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     0.0283169,
     1,
     0},
    {"cubic inch",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     1.63871e-005,
     1,
     0},
    {"cubic yard",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     0.764555,
     1,
     0},
    {"cubic mile",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     4.16818e+009,
     1,
     0},
    {"cheval vapeur",
     {"power", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     735.499,
     1,
     0},
    {"CV hours",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     2.6478e+006,
     1,
     0},
    {"UK hundredweight",
     {"mass", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     53,
     RescueUOM::FACTOR,
     0,
     50.8023,
     1,
     0},
    {"US hundredweight",
     {"mass", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     53,
     RescueUOM::FACTOR,
     0,
     45.3592,
     1,
     0},
    {"darcy",
     {"permeability rock", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     9.86923e-013,
     1,
     0},
    {"day", {"time", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 121, RescueUOM::FACTOR, 0, 86400, 1, 0},
    {"darcy foot",
     {"permeability length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     3.00814e-013,
     1,
     0},
    {"darcy metre",
     {"permeability length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     9.86923e-013,
     1,
     0},
    {"day per barrel",
     {"time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"days/cubic foot",
     {"time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"day per thousand cubic feet",
     {"time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"days/cubic metre",
     {"time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"decanewtons", {"force", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 96, RescueUOM::FACTOR, 0, 10, 1, 0},
    {"decanewton metres",
     {"moment of force", "moment of couple", "torque", "", "", "", "", "", "", "", "", "", ""},
     3,
     41,
     RescueUOM::FACTOR,
     0,
     10,
     1,
     0},
    {"API gravity",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"decibel",
     {"level of power intensity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     14,
     RescueUOM::FACTOR,
     0,
     0.1,
     1,
     0},
    {"decibels/foot",
     {"attenuation per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"decibels/metre",
     {"attenuation per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"decibels/kilometre",
     {"attenuation per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"decibels/octave",
     {"attenuation", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"change in degrees Celsius",
     {"delta temperature", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     48,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"change in degrees Fahrenheit",
     {"delta temperature", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     48,
     RescueUOM::FRACTION,
     0,
     5,
     9,
     0},
    {"change in degrees Kelvin",
     {"delta temperature", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     48,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"change in degrees Rankine",
     {"delta temperature", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     48,
     RescueUOM::FRACTION,
     0,
     5,
     9,
     0},
    {"degree of an angle",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FACTOR,
     0,
     0.0174533,
     1,
     0},
    {"degrees of an angle/100 feet",
     {"angle per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees of an angle per thirty feet",
     {"angle per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees of an angle/30 metres",
     {"angle per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees of an angle/foot",
     {"angle per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees of an angle per hour",
     {"rotational frequency", "circular frequency", "angle per time", "rotational velocity", "", "", "", "", "", "", "",
      "", ""},
     4,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees of an angle/metre",
     {"angle per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees of an angle/minute",
     {"rotational frequency", "circular frequency", "angle per time", "rotational velocity", "", "", "", "", "", "", "",
      "", ""},
     4,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees of an angle per second",
     {"rotational frequency", "circular frequency", "angle per time", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Celsius",
     {"thermodynamic temperature", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     48,
     RescueUOM::NONE,
     273.15,
     1,
     1,
     0},
    {"degrees C square metres hours/kilocal",
     {"thermal insulance", "coefficient of thermal insulation", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Celsius per hundred metre",
     {"temperature per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Celsius per foot",
     {"temperature per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Celsius per hour",
     {"temperature per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Celsius/kilometre",
     {"temperature per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Celsius/metre",
     {"temperature per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Celsius per minute",
     {"temperature per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Celsius per second",
     {"temperature per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degree Fahrenheit",
     {"thermodynamic temperature", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     48,
     RescueUOM::NONE,
     2298.35,
     5,
     9,
     0},
    {"degrees F square feet hours/Btu",
     {"thermal insulance", "coefficient of thermal insulation", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Fahrenheit/100 feet.",
     {"temperature per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Fahrenheit/foot",
     {"temperature per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Fahrenheit per hour",
     {"temperature per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Fahrenheit per meter",
     {"temperature per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Fahrenheit per minute",
     {"temperature per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Fahrenheit per second",
     {"temperature per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Rankine",
     {"thermodynamic temperature", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     48,
     RescueUOM::FRACTION,
     0,
     5,
     9,
     0},
    {"decimetre",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "Cartesian coordinates", "diameter",
      "length of path", "distance", "wavelength", "mean free path", ""},
     12,
     69,
     RescueUOM::FACTOR,
     0,
     0.1,
     1,
     0},
    {"decimeter per second",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic decimetre",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"cubic decimetres/100 kilometres",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     1e-008,
     1,
     0},
    {"cubic decimetres/kilogram",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic decimetres/kilowatt hour",
     {"isothermal compressibility", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic decimetres/metre",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"cubic decimetres/cubic metre",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"cubic decimetres/megajoule",
     {"isothermal compressibility", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic decimetres/kilogram mole",
     {"molar volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic decimetres/second",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic decimetres/second/second",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic decimetres/ton",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"decinewton metres",
     {"moment of force", "moment of couple", "torque", "", "", "", "", "", "", "", "", "", ""},
     3,
     41,
     RescueUOM::FACTOR,
     0,
     0.1,
     1,
     0},
    {"dynes", {"force", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 96, RescueUOM::FACTOR, 0, 1e-005, 1, 0},
    {"dyne centimetre squared",
     {"force area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"dyne seconds/square centimetre",
     {"dynamic viscosity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"dynes/centimetre",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"dynes/square centimetre",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     0.1,
     1,
     0},
    {"dynes/centimetre fourth/gram cm cubed",
     {"parachor", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"newton/metre fourth/kilogram metre cubed",
     {"parachor", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"electric horsepower",
     {"power", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     746,
     1,
     0},
    {"exajoule",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1e+018,
     1,
     0},
    {"exajoules/year",
     {"heat flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     3.16875e+010,
     1,
     0},
    {"equivalents/ Liter",
     {"equivalent per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"ergs", {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""}, 2, 41, RescueUOM::FACTOR, 0, 1e-007, 1, 0},
    {"ergs/year",
     {"heat flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FRACTION,
     0,
     1e-007,
     3.15582e+007,
     0},
    {"ergs/square centimetre",
     {"energy per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"ergs/cubic centimetre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"ergs/gram",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"ergs/kilogram",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"ergs/cubic metre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"electron volts",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1.60218e-019,
     1,
     0},
    {"fathoms",
     {"depth", "length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     69,
     RescueUOM::FACTOR,
     0,
     1.8288,
     1,
     0},
    {"femtocoulomb",
     {"electric capacity", "electric charge", "electric flux", "", "", "", "", "", "", "", "", "", ""},
     3,
     22,
     RescueUOM::FACTOR,
     0,
     1e-015,
     1,
     0},
    {"UK fluid ounce",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     2.84131e-005,
     1,
     0},
    {"US fluid ounces",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     2.95735e-005,
     1,
     0},
    {"flops",
     {"operations per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"femtometer",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "Cartesian coordinates", "diameter",
      "length of path", "distance", "wavelength", "mean free path", ""},
     12,
     69,
     RescueUOM::FACTOR,
     0,
     1e-015,
     1,
     0},
    {"footcandles",
     {"illuminance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     67,
     RescueUOM::FACTOR,
     0,
     10.7639,
     1,
     0},
    {"footcandle seconds",
     {"light exposure", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"foot",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "Cartesian coordinates", "diameter",
      "length of path", "distance", "wavelength", "mean free path", ""},
     12,
     69,
     RescueUOM::FACTOR,
     0,
     0.3048,
     1,
     0},
    {"foot pounds force",
     {"moment of force", "moment of couple", "torque", "", "", "", "", "", "", "", "", "", ""},
     3,
     41,
     RescueUOM::FACTOR,
     0,
     1.35582,
     1,
     0},
    {"foot pounds force/barrel",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"foot pounds force/US gallon",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"foot pounds force/pound mass",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"foot pounds force/minute",
     {"heat flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     0.022597,
     1,
     0},
    {"foot pounds force/second",
     {"heat flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     1.35582,
     1,
     0},
    {"foot-pound mass",
     {"mass length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"feet per 100 feet",
     {"linear strain", "relative elongation", "scale", "linear concentration", "length per length", "", "", "", "", "",
      "", "", ""},
     5,
     32,
     RescueUOM::FACTOR,
     0,
     0.01,
     1,
     0},
    {"feet/barrel",
     {"length per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"feet/day", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"feet/degree Fahrenheit",
     {"length per temperature", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"feet per feet",
     {"linear strain", "relative elongation", "scale", "linear concentration", "length per length", "", "", "", "", "",
      "", "", ""},
     5,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"feet/cubic foot",
     {"length per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"feet/US gallon",
     {"length per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"feet/hour", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"feet/inch",
     {"scale", "length per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     12,
     1,
     0},
    {"feet/metre",
     {"linear strain", "relative elongation", "scale", "linear concentration", "length per length", "", "", "", "", "",
      "", "", ""},
     5,
     32,
     RescueUOM::FACTOR,
     0,
     0.3048,
     1,
     0},
    {"feet/mile",
     {"linear strain", "relative elongation", "scale", "linear concentration", "length per length", "", "", "", "", "",
      "", "", ""},
     5,
     32,
     RescueUOM::FRACTION,
     0,
     1,
     5280,
     0},
    {"feet/minute", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"foot per millisecond",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"feet/second", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"feet/second squared",
     {"acceleration linear", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"foot per microsecond",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square foot",
     {"area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     0.092903,
     1,
     0},
    {"square feet/hour",
     {"kinematic viscosity", "thermal diffusivity", "area per time", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square feet/cubic inch",
     {"area per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square feet/second",
     {"kinematic viscosity", "thermal diffusivity", "area per time", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet at standard conditions",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     128,
     RescueUOM::FACTOR,
     0,
     0.0011953,
     1,
     0},
    {"cubic feet/barrel",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     0.0283169,
     0.158987,
     0},
    {"cubic feet/day",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/day foot psi",
     {"unit productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/day/day",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/foot",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     0.092903,
     1,
     0},
    {"cubic feet/cubic foot",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"cubic feet/hour",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/hour/hour",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet per kilogram",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/pound mass",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/minute",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/min square foot",
     {"volume per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/minute/minute",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/mole (pound mass)",
     {"molar volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/second",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/second square foot",
     {"volume per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/second/second",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet per 94 pound sack",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic feet/std cubic foot, 60 deg F",
     {"volume per standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"British Foot (Benoit 1895 A)",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     0.914399,
     3,
     0},
    {"British Foot (Benoit 1895 B)",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     12,
     39.3701,
     0},
    {"British Foot 1865",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     0.914403,
     3,
     0},
    {"Imperial Foot",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     0.304797,
     1,
     0},
    {"Gold Coast Foot",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     6.3783e+006,
     2.09262e+007,
     0},
    {"Indian Foot",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     12,
     39.3701,
     0},
    {"Indian Foot, 1937",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     0.304798,
     1,
     0},
    {"Indian Foot, 1962",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     0.3048,
     1,
     0},
    {"Indian Foot, 1975",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     0.3048,
     1,
     0},
    {"Modified American Foot",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     0.304812,
     1,
     0},
    {"Sears Foot",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     12,
     39.3701,
     0},
    {"US Survey Foot",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     12,
     39.37,
     0},
    {"gram", {"mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 53, RescueUOM::FACTOR, 0, 0.001, 1, 0},
    {"gram feet/cubic centimetre second",
     {"mass per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"grams/cubic centimetre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"grams/centimetre fourth",
     {"mass per volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"grams/cubic decimetre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"grams/UK gallon",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"grams/US gallon",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"grams/kilogram",
     {"mass concentration", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"grams/litre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"grams/cubic metre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"grams/second",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"gigayears",
     {"time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     121,
     RescueUOM::FACTOR,
     0,
     3.15582e+016,
     1,
     0},
    {"galileo",
     {"acceleration linear", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/94 lb sack",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK gallon",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     0.00454609,
     1,
     0},
    {"UK gallons per day",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK gallons/cubic foot",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.160544,
     1,
     0},
    {"UK gallons/hour",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK gallons/hour foot",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK gallons/hour square foot",
     {"volume per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK gallons/hour inch",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK gallons/hour square inch",
     {"volume per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK gallons/hour/hour",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK gallons per thousand UK gallons",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"UK gallons/pound mass",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK gallons/1000 barrels",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     2.85941e-005,
     1,
     0},
    {"UK gallons/mile",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FRACTION,
     0,
     0.00454609,
     1609.34,
     0},
    {"UK gallons/minute",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK gallons/minute foot",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK gallons/minute square foot",
     {"volume per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK gallons/minute/minute",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     0.00378541,
     1,
     0},
    {"US gallons per ten barrels",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.00238095,
     1,
     0},
    {"US gallons/barrels",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.0238095,
     1,
     0},
    {"US gallons per day",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/foot",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FRACTION,
     0,
     0.00378541,
     0.3048,
     0},
    {"US gallons/cubic foot",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.133681,
     1,
     0},
    {"US gallons/hour",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/foot hour",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/hour square foot",
     {"volume per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/hour inch",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/hour square inch",
     {"volume per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/hour/hour",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons per thousand US gallons",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"US gallons/pound mass",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/1000 barrels",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     2.38095e-005,
     1,
     0},
    {"US gallons/mile",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FRACTION,
     0,
     0.00378541,
     1609.34,
     0},
    {"US gallons/minute",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/minute foot",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/minute square foot",
     {"volume per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/minute/minute",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gals/1000 std cubic feet, 60 deg F",
     {"volume per standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/UK ton",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US gallons/US ton",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"gamma",
     {"linear electric current density", "magnetic field strength", "magnetization", "", "", "", "", "", "", "", "", "",
      ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"gauss",
     {"magnetic induction", "magnetic flux density", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     135,
     RescueUOM::FACTOR,
     0,
     0.0001,
     1,
     0},
    {"gigabecquerel",
     {"activity (of radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     19,
     RescueUOM::FACTOR,
     0,
     1e+009,
     1,
     0},
    {"billions of electron volts",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1.60219e-010,
     1,
     0},
    {"gram force",
     {"force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     0.00980665,
     1,
     0},
    {"gigahertz", {"frequency", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"gigajoule",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1e+009,
     1,
     0},
    {"gravity",
     {"acceleration linear", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"gigaohm",
     {"resistance", "impedance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     103,
     RescueUOM::FACTOR,
     0,
     1e+009,
     1,
     0},
    {"gons",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FACTOR,
     0,
     0.015708,
     1,
     0},
    {"gigapascal",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     1e+009,
     1,
     0},
    {"gigapascal per centimeter",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"gigapascal squared",
     {"pressure squared", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     113,
     RescueUOM::FACTOR,
     0,
     1e+018,
     1,
     0},
    {"grad",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FACTOR,
     0,
     0.015708,
     1,
     0},
    {"gigaradian",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FACTOR,
     0,
     1e+009,
     1,
     0},
    {"grain",
     {"mass", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     53,
     RescueUOM::FACTOR,
     0,
     6.47989e-005,
     1,
     0},
    {"grains/100 cubic feet",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"grains/cubic foot",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"grains/US gallon",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"gigasiemens",
     {"electric conductance", "admittance", "susceptance", "", "", "", "", "", "", "", "", "", ""},
     3,
     120,
     RescueUOM::FACTOR,
     0,
     1e+009,
     1,
     0},
    {"giga standard cubic metres 15C",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     128,
     RescueUOM::FACTOR,
     0,
     1e+009,
     1,
     0},
    {"gigawatt",
     {"power", "heat flow rate", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     139,
     RescueUOM::FACTOR,
     0,
     1e+009,
     1,
     0},
    {"gigawatt hour",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     3.6e+012,
     1,
     0},
    {"hour", {"time", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 121, RescueUOM::FACTOR, 0, 3600, 1, 0},
    {"hours/cubic foot",
     {"time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"hour per thousand foot",
     {"time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"hour per kilometer",
     {"time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"hour per cubic meter",
     {"time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"hectare", {"area", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 73, RescueUOM::FACTOR, 0, 10000, 1, 0},
    {"hectare metres",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     10000,
     1,
     0},
    {"hectobar",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     1e+007,
     1,
     0},
    {"hydraulic horsepower",
     {"power", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     746.043,
     1,
     0},
    {"(hydraulic) horsepower per square inch",
     {"density of heat flow rate", "poynting vector", "radiant energy", "fluence rate", "radiant exitance",
      "irradiance", "sound intensity", "energy fluence rate", "", "", "", "", ""},
     8,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"hectoliter", {"volume", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 78, RescueUOM::FACTOR, 0, 0.1, 1, 0},
    {"horsepower",
     {"power", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     745.7,
     1,
     0},
    {"horsepower hour",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     2.68452e+006,
     1,
     0},
    {"horsepower hours/barrel",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"horsepower hours/pound mass",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"horsepower/cubic foot",
     {"power per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"horsepower per square inch",
     {"density of heat flow rate", "poynting vector", "radiant energy", "fluence rate", "radiant exitance",
      "irradiance", "sound intensity", "energy fluence rate", "", "", "", "", ""},
     8,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"hundred seconds",
     {"time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     121,
     RescueUOM::FACTOR,
     0,
     100,
     1,
     0},
    {"inch",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "Cartesian coordinates", "diameter",
      "length of path", "distance", "wavelength", "mean free path", ""},
     12,
     69,
     RescueUOM::FACTOR,
     0,
     0.0254,
     1,
     0},
    {"tenth of an inch",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "Cartesian coordinates", "diameter",
      "", "", "", "", ""},
     8,
     69,
     RescueUOM::FACTOR,
     0,
     0.00254,
     1,
     0},
    {"16th of an inch",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "Cartesian coordinates", "diameter",
      "", "", "", "", ""},
     8,
     69,
     RescueUOM::FACTOR,
     0,
     0.0015875,
     1,
     0},
    {"32nd of an inch",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "Cartesian coordinates", "diameter",
      "", "", "", "", ""},
     8,
     69,
     RescueUOM::FACTOR,
     0,
     0.00079375,
     1,
     0},
    {"64th of an inch",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "Cartesian coordinates", "diameter",
      "", "", "", "", ""},
     8,
     69,
     RescueUOM::FACTOR,
     0,
     0.000396875,
     1,
     0},
    {"inches/year", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"inches/inch degree Fahrenheit",
     {"linear thermal expansion", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"inches/minute", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"inches/second", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"square inches",
     {"area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     0.00064516,
     1,
     0},
    {"square inches/square foot",
     {"area per area", "shear strain", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FRACTION,
     0,
     1,
     144,
     0},
    {"square inches/square inch",
     {"area per area", "shear strain", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"square inches/second",
     {"kinematic viscosity", "thermal diffusivity", "area per time", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic inches",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     1.63871e-005,
     1,
     0},
    {"cubic inches/foot",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FRACTION,
     0,
     1.63871e-005,
     0.3048,
     0},
    {"inches to the fourth",
     {"second moment of area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     89,
     RescueUOM::FACTOR,
     0,
     4.16231e-007,
     1,
     0},
    {"inches of water at 39.2 deg F",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     249.082,
     1,
     0},
    {"inches of water at 60 deg F",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     248.84,
     1,
     0},
    {"inches of mercury at 32 deg F",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     3386.38,
     1,
     0},
    {"inches of mercury at 60 deg F",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     3376.85,
     1,
     0},
    {"US Survey inch",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     1,
     39.37,
     0},
    {"joules/square centimetre",
     {"energy per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"joules/cubic decimetre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"joules/gram",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"joules/gram degree Kelvin",
     {"specific heat capacity", "massic heat capacity", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"joules/metre",
     {"energy per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"joules/square metre",
     {"energy per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"joules/second square metre deg C",
     {"heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"degrees Kelvin square metres/kilowatt",
     {"thermal insulance", "coefficient of thermal insulation", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kiloampere",
     {"electric current", "magnetic potential difference", "magnetomotive force", "", "", "", "", "", "", "", "", "",
      ""},
     3,
     10,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"thousand barrels per day",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilobyte",
     {"digital storage", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     21,
     RescueUOM::FACTOR,
     0,
     1024,
     1,
     0},
    {"kilocoulombs",
     {"electric capacity", "electric charge", "electric flux", "", "", "", "", "", "", "", "", "", ""},
     3,
     22,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilocalories",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     4184,
     1,
     0},
    {"kilocalorie metres/square centimetre",
     {"energy length per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FRACTION,
     0,
     4184,
     0.0001,
     0},
    {"kilocalories/cubic centimetre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilocalories/gram",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilocalories/hour",
     {"power", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     1.16222,
     1,
     0},
    {"kilocalories/hour metre degree Celsius",
     {"thermal conductivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilocalorie/hour square metre deg C",
     {"heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilocalories/kilogram",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilocalories/kilogram degree Celsius",
     {"specific heat capacity", "massic heat capacity", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilocalroies/cubic metre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilocalories/mole (gram)",
     {"molar thermodynamic energy", "chemical potential", "affinity of a chemical reaction", "", "", "", "", "", "", "",
      "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilocandela",
     {"luminous intensity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     27,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilkodynes", {"force", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 96, RescueUOM::FACTOR, 0, 0.01, 1, 0},
    {"thousand per second",
     {"per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kiloelectron volts",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1.60218e-016,
     1,
     0},
    {"thousand foot pounds force",
     {"moment of force", "moment of couple", "torque", "", "", "", "", "", "", "", "", "", ""},
     3,
     41,
     RescueUOM::FACTOR,
     0,
     1.35582e+006,
     1,
     0},
    {"thousand feet per hour",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand feet per second",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilogram metres/square centimetre",
     {"mass per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilogram per day",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms/cubic decimetre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms/decimetre fourth",
     {"mass per volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms/hour",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms/kilogram",
     {"mass concentration", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"kilograms/kilowatt hour",
     {"mass per energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilogram per litre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms/metre second",
     {"mass per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilogram per min",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms/megajoule",
     {"mass per energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilogram per 94 pound sack",
     {"mass concentration", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     1,
     42.6377,
     0},
    {"kilogram force",
     {"force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     9.80665,
     1,
     0},
    {"kilogram force metres",
     {"moment of force", "moment of couple", "torque", "", "", "", "", "", "", "", "", "", ""},
     3,
     41,
     RescueUOM::FACTOR,
     0,
     9.80665,
     1,
     0},
    {"kilogram force metres/square centimetre",
     {"energy per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms force metres/metre",
     {"force length per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     9.80665,
     1,
     0},
    {"kilogram force metres squared",
     {"force area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms force seconds/square metre",
     {"dynamic viscosity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilograms force/centimetre",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilogram per square centimeter",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FRACTION,
     0,
     9.80665,
     0.0001,
     0},
    {"kilogram force per kilogram force",
     {"force per force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"kilogram force/square millimetre",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FRACTION,
     0,
     9.80665,
     1e-006,
     0},
    {"kilohertz", {"frequency", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"kilojoules",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilojoule metres/hour sq metre deg K",
     {"energy length per time area temperature", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilojoules/cubic decimetre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilojoules/hour square metre deg K",
     {"heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilojoule/kilogram",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilojoules/kilogram degree Kelvin",
     {"specific heat capacity", "massic heat capacity", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilojoule/cubic metre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilojoule/mole (kilogram)",
     {"molar thermodynamic energy", "chemical potential", "affinity of a chemical reaction", "", "", "", "", "", "", "",
      "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilojoules/mole (kilogram) deg K",
     {"molar heat capacity", "molar entropy", "molar gas constant", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand pounds force",
     {"force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     4448.22,
     1,
     0},
    {"thousand pounds mass",
     {"mass", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     53,
     RescueUOM::FACTOR,
     0,
     453.592,
     1,
     0},
    {"thousand pounds mass per inch",
     {"mass per length", "linear density", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilolux",
     {"illuminance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     67,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilometre",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "Cartesian coordinates", "diameter",
      "length of path", "distance", "wavelength", "mean free path", ""},
     12,
     69,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilometre/ centimetre",
     {"scale", "length per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     100000,
     1,
     0},
    {"kilometres/cubic decimetre",
     {"length per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilometres/hour",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilometres/litre",
     {"length per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilometer per second",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square kilometres",
     {"area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"cubic kilometres",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     1e+009,
     1,
     0},
    {"kilomole",
     {"amount of substance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     91,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilonewtons", {"force", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 96, RescueUOM::FACTOR, 0, 1000, 1, 0},
    {"kilonewton metres",
     {"moment of force", "moment of couple", "torque", "", "", "", "", "", "", "", "", "", ""},
     3,
     41,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilonewton metres squared",
     {"force area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilonewtons/metre",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilonewtons/square metre",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"knots", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"kilohm",
     {"resistance", "impedance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     103,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilo ohm metre",
     {"electrical resistivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilopascals",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilopascal seconds/metre",
     {"mass per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilopascal per hundred meter",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilopascal per hour",
     {"pressure per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilopascals/metre",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilopascal per min",
     {"pressure per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilopascal squared",
     {"pressure squared", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     113,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"kilopascal squared per centipoise",
     {"pressure squared per (dynamic viscosity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilopascal squared per thousand centipoise",
     {"pressure squared per (dynamic viscosity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand pounds per square inch",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     6.89476e+006,
     1,
     0},
    {"thousand pound per square inch, squared",
     {"pressure squared", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     113,
     RescueUOM::FACTOR,
     0,
     4.75377e+013,
     1,
     0},
    {"kiloradian",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilosiemens",
     {"electric conductance", "admittance", "susceptance", "", "", "", "", "", "", "", "", "", ""},
     3,
     120,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilo standard cubic metres 15C",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     128,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"thousand std cubic metres/ day",
     {"standard volume per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"1000 std cubic metres/ std cubic metre",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilovolt",
     {"electric potential", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     136,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilowatts",
     {"power", "heat flow rate", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     139,
     RescueUOM::FACTOR,
     0,
     1000,
     1,
     0},
    {"kilowatt hours",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     3.6e+006,
     1,
     0},
    {"kilowatt hours/decimetre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilowatt hours/kilogram",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilowatt hours/kilogram degree C",
     {"specific heat capacity", "massic heat capacity", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilowatt hours/cubic metres",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilowatts/square centimetre",
     {"density of heat flow rate", "poynting vector", "radiant energy", "fluence rate", "radiant exitance",
      "irradiance", "sound intensity", "energy fluence rate", "", "", "", "", ""},
     8,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilowatts/square metre",
     {"density of heat flow rate", "poynting vector", "radiant energy", "fluence rate", "radiant exitance",
      "irradiance", "sound intensity", "energy fluence rate", "", "", "", "", ""},
     8,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilowatts/square metre degree Kelvin",
     {"heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"kilowatts/cubic metre",
     {"power per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"killowatts/cubic metre degree Kelvin",
     {"volumetric heat transfer coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"litre", {"volume", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 78, RescueUOM::FACTOR, 0, 0.001, 1, 0},
    {"liter per hundred kilogram",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"litres/100 kilometres",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"liter per ten barrel",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     0.001,
     1.58987,
     0},
    {"litres per minute per bar",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"liter per hour",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"liter per kilogram",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"litres/metre",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"litres/cubic metre",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"liter per minute",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"litres/mole (gram)",
     {"molar volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"litres/mole (kilogram)",
     {"molar volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"litres/second",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"litres/second/second",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"litres/tonne",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"liter per UK ton",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds force",
     {"force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     4.44822,
     1,
     0},
    {"pounds force feet/inch",
     {"force length per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FRACTION,
     0,
     1.35582,
     0.0254,
     0},
    {"foot pounds force/square inch",
     {"energy per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"inch pounds force",
     {"moment of force", "moment of couple", "torque", "", "", "", "", "", "", "", "", "", ""},
     3,
     41,
     RescueUOM::FACTOR,
     0,
     0.112985,
     1,
     0},
    {"pounds force inches/inch",
     {"force length per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     4.44822,
     1,
     0},
    {"pounds force inches squared",
     {"force area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds force seconds/square foot",
     {"dynamic viscosity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds force seconds/square inch",
     {"dynamic viscosity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds force per hundred foot",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds force/100 square foot",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     0.478803,
     1,
     0},
    {"pounds force per thirty meters",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds force per foot",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds force/square foot",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     47.8803,
     1,
     0},
    {"pounds force/cubic foot",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds force/US gallon",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds force/inch",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds force/square inch",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     6894.76,
     1,
     0},
    {"pound force per pound force",
     {"force per force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"pounds mass",
     {"mass", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     53,
     RescueUOM::FACTOR,
     0,
     0.453592,
     1,
     0},
    {"foot pounds mass/second",
     {"momentum", "impulse", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass square feet",
     {"moment of inertia", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass square feet/second squared",
     {"moment of force", "moment of couple", "torque", "", "", "", "", "", "", "", "", "", ""},
     3,
     41,
     RescueUOM::FACTOR,
     0,
     0.0421401,
     1,
     0},
    {"million pounds mass/year",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/1000 UK gallons",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/1000 US gallons",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass per hundred square foot",
     {"surface density", "areic mass", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass per 10 barrel",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/barrel",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pound mass per day",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/foot",
     {"mass per length", "linear density", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/foot hour",
     {"mass per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/foot second",
     {"mass per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/square foot",
     {"surface density", "areic mass", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/cubic foot",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/foot fourth",
     {"mass per volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/UK gallon",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/UK gallon foot",
     {"mass per volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/US gallon",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/US gallon foot",
     {"mass per volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/hour",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/hour foot",
     {"mass per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/hour square foot",
     {"mass per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/horsepower hour",
     {"mass per energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/cubic inch",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/1000 barrels",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/minute",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/second",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/second foot",
     {"mass per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds mass/second square foot",
     {"mass per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"British link 1895 A",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     0.201168,
     1,
     0},
    {"British link 1895 B",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     7.92,
     39.3701,
     0},
    {"Clarke link",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     7.92,
     39.3704,
     0},
    {"Sears link",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     7.92,
     39.3701,
     0},
    {"US Survey link",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     7.92,
     39.37,
     0},
    {"lumens/square metre",
     {"luminous exitance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     67,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"million cubic feet",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     28316.8,
     1,
     0},
    {"million cubic feet per acre-foot",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     1e+006,
     43560,
     0},
    {"million cubic feet per day",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"million cubic meters",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"million cubic metres per day",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"metres per thirty metres",
     {"linear strain", "relative elongation", "scale", "linear concentration", "length per length", "", "", "", "", "",
      "", "", ""},
     5,
     32,
     RescueUOM::FRACTION,
     0,
     1,
     30,
     0},
    {"metres/ centimetre",
     {"scale", "length per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     100,
     1,
     0},
    {"metres/day", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"metres/hour", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"metres/kilometre",
     {"linear strain", "relative elongation", "scale", "linear concentration", "length per length", "", "", "", "", "",
      "", "", ""},
     5,
     32,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"metres/metre",
     {"linear strain", "relative elongation", "scale", "linear concentration", "length per length", "", "", "", "", "",
      "", "", ""},
     5,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"metres/metre Kelvin",
     {"linear thermal expansion", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"metres/cubic metre",
     {"length per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"meter per minute",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"metres/millisecond",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square metres/cubic centimetre",
     {"area per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square metres/day kiloPascal",
     {"unit productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square metres/gram",
     {"mass attenuation coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square metres/hour",
     {"kinematic viscosity", "thermal diffusivity", "area per time", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square metres/square metre",
     {"area per area", "shear strain", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"square metres/cubic metre",
     {"area per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres at std condition (0 deg C)",
     {"amount of substance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     91,
     RescueUOM::FACTOR,
     0,
     0.0446158,
     1,
     0},
    {"cubic metres at std condition (15 deg C)",
     {"amount of substance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     91,
     RescueUOM::FACTOR,
     0,
     0.0422932,
     1,
     0},
    {"cubic meter per day per bar",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic meter per hour per bar",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic meter per minute per bar",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/centiPoise day kiloPascal",
     {"specific productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/centiPoise Pascal second",
     {"specific productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/day",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/day kilopascal",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic meter per day per meter",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/day/day",
     {"volume per time per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/gram",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/hour",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic meter per hour per meter",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/hectare metre",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.0001,
     1,
     0},
    {"cubic metres/kilometre",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"cubic meter per day per kilopascal",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"(cubic metres per hour) per kilopascal",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/kilowatt hour",
     {"isothermal compressibility", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/metre",
     {"volume per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"cubic meter per square meter",
     {"volume per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     69,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"cubic metres/cubic metre",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"cubic meter per minute",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/mole (kilogram)",
     {"molar volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic meter per day per (pound per square inch)",
     {"productivity index", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic meter per second per foot",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/second metre",
     {"volume per time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/second square metre",
     {"volume per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic metres/tonne",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic meters per UK ton",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic meters per US ton",
     {"specific volume", "massic volume", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milliamp",
     {"electric current", "magnetic potential difference", "magnetomotive force", "", "", "", "", "", "", "", "", "",
      ""},
     3,
     10,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"megayears",
     {"time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     121,
     RescueUOM::FACTOR,
     0,
     3.15582e+013,
     1,
     0},
    {"megaampere",
     {"electric current", "magnetic potential difference", "magnetomotive force", "", "", "", "", "", "", "", "", "",
      ""},
     3,
     10,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"milliampere per square centimeter",
     {"current density", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milliampere per square foot",
     {"current density", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millibar",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     100,
     1,
     0},
    {"thousand barrels",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     158.987,
     1,
     0},
    {"thousand barrel feet/day",
     {"volume length per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand barrels/day",
     {"volume flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megabecquerel",
     {"activity (of radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     19,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"megabyte",
     {"digital storage", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     21,
     RescueUOM::FACTOR,
     0,
     1.04858e+006,
     1,
     0},
    {"millicoulomb",
     {"electric capacity", "electric charge", "electric flux", "", "", "", "", "", "", "", "", "", ""},
     3,
     22,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"millicoulombs/square metre",
     {"electric polarization", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millicurie",
     {"activity (of radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     19,
     RescueUOM::FACTOR,
     0,
     37000,
     1,
     0},
    {"millidarcy",
     {"permeability rock", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     9.86932e-016,
     1,
     0},
    {"millidarcy foot",
     {"permeability length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     3.00814e-016,
     1,
     0},
    {"millidarcy sq feet/pound force second",
     {"mobility", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millidarcy sq inches/pound force second",
     {"mobility", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millidarcy metres",
     {"permeability length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     9.86932e-016,
     1,
     0},
    {"millidarcies/centipoise",
     {"mobility", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millidarcies/Pascal second",
     {"mobility", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milliequivalent",
     {"electrochemical equivalent", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     29,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"milliequivalents/ hectogram",
     {"equivalent per mass", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milliequivalents/ cubic centimeter",
     {"equivalent per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milliequivalents/ gram",
     {"equivalent per mass", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milliEuclid",
     {"dimensionless", "poisson ratio", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"millions of electron volts",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1.60219e-013,
     1,
     0},
    {"megaflops",
     {"operations per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megagram", {"mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 53, RescueUOM::FACTOR, 0, 1000, 1, 0},
    {"milligram", {"mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 53, RescueUOM::FACTOR, 0, 1e-006, 1, 0},
    {"megagrams/year",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megagrams/day",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milligrams/cubic decimetre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milligrams/US gallon",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megagrams/hour",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand kilograms per inch",
     {"mass per length", "linear density", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milligrams/joule",
     {"mass per energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milligrams/kilogram",
     {"mass concentration", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"milligram per litre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megagrams/square metre",
     {"surface density", "areic mass", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milligrams/cubic metre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand kilograms per cubic metre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milligalileo",
     {"acceleration linear", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milligauss",
     {"magnetic induction", "magnetic flux density", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     135,
     RescueUOM::FACTOR,
     0,
     1e-007,
     1,
     0},
    {"German legal metre",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     1.00001,
     1,
     0},
    {"thousand kilograms force",
     {"force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     4.44822e+006,
     1,
     0},
    {"milligravity",
     {"acceleration linear", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milligray",
     {"absorbed dose", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     37,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"millihenries",
     {"self inductance", "permeance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     38,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"mhos",
     {"electric conductance", "admittance", "susceptance", "", "", "", "", "", "", "", "", "", ""},
     3,
     120,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"mhos/metre",
     {"conductivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megahertz", {"frequency", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"millihertz", {"frequency", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"mile",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "Cartesian coordinates", "diameter",
      "length of path", "distance", "wavelength", "", ""},
     11,
     69,
     RescueUOM::FACTOR,
     0,
     1609.34,
     1,
     0},
    {"miles/UK gallon",
     {"length per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"miles/US gallon",
     {"length per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"miles/hour", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"miles/inch",
     {"scale", "length per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     63360,
     1,
     0},
    {"square miles",
     {"area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     2.58999e+006,
     1,
     0},
    {"mil, a thousandth of an inch",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "diameter", "length of path",
      "distance", "wavelength", "mean free path", "", ""},
     11,
     69,
     RescueUOM::FACTOR,
     0,
     2.54e-005,
     1,
     0},
    {"mils/year", {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"mil_6400",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FRACTION,
     0,
     3.14159,
     3200,
     0},
    {"minutes", {"time", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 121, RescueUOM::FACTOR, 0, 60, 1, 0},
    {"minute per foot",
     {"time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"minute per meter",
     {"time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"minutes angular",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FACTOR,
     0,
     0.000290888,
     1,
     0},
    {"U.S. Survey mile",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     1609.35,
     1,
     0},
    {"U.S. Survey square mile",
     {"area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     2.59e+006,
     1,
     0},
    {"megajoules",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"millijoules",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"megajoules/year",
     {"power", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FRACTION,
     0,
     1e+006,
     3.15582e+007,
     0},
    {"millijoules/square centimetre",
     {"energy per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megajoules/kilogram",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megajoules/metre",
     {"energy per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"millijoules/square metre",
     {"energy per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megajoules/cubic metre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megajoules/mole (kilogram)",
     {"molar thermodynamic energy", "chemical potential", "affinity of a chemical reaction", "", "", "", "", "", "", "",
      "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millidegrees Kelvin/metre",
     {"temperature per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millilitre",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"millilitres/UK gallon",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.000219969,
     1,
     0},
    {"millilitres/US gallon",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.000264172,
     1,
     0},
    {"milliliter per milliliter",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"millimetres",
     {"length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     69,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"megameter", {"length", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 69, RescueUOM::FACTOR, 0, 0.001, 1, 0},
    {"millimetres/year",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millimetres/millimetre degree Kelvin",
     {"linear thermal expansion", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millimetres/second",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square millimetres",
     {"area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"square millimetres/square millimetre",
     {"area per area", "shear strain", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"square millimetres/second",
     {"kinematic viscosity", "thermal diffusivity", "area per time", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"cubic millimetres",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"cubic millimetres/joule",
     {"isothermal compressibility", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"million barrels",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     158987,
     1,
     0},
    {"million barrels/acre foot",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     158987,
     1233.49,
     0},
    {"millimetres of Mercury at 0 deg C",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     133.322,
     1,
     0},
    {"millimhos/metre",
     {"conductivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millimole",
     {"amount of substance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     91,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"million standard cubic feet at 60 deg F",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     128,
     RescueUOM::FACTOR,
     0,
     28262.4,
     1,
     0},
    {"million standard cubic feet/day",
     {"standard volume per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"million std cu ft/ stock tank barrel",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     28262.4,
     0.158987,
     0},
    {"million standard cubic metres 15C",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     128,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"million std cubic metres, 15 degC/day",
     {"standard volume per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"million stock tank barrels 60 deg F",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     128,
     RescueUOM::FACTOR,
     0,
     158987,
     1,
     0},
    {"million stock tank barrels, 60 deg F/day",
     {"standard volume per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"million stock tank barrels 60 deg F/acre",
     {"standard volume per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"million stbs, 60 deg F/acre foot",
     {"standard volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"meganewtons",
     {"force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"millinewtons",
     {"force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"millinewton metres squared",
     {"force area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millinewtons/kilometre",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millinewtons/metre",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megaohm",
     {"resistance", "impedance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     103,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"milliohm",
     {"resistance", "impedance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     103,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"mole (gram)",
     {"amount of substance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     91,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"mole (kilogram)",
     {"amount of substance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     91,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"moles (kilogram)/hour",
     {"mole per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles (kilogram)/cubic metre",
     {"concentration of B", "amount of a substance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles (kilogram)/second",
     {"mole per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles (pounds mass)",
     {"amount of substance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     91,
     RescueUOM::FACTOR,
     0,
     453.592,
     1,
     0},
    {"moles (pounds mass)/cubic foot",
     {"concentration of B", "amount of a substance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles (pounds mass)/UK gallon",
     {"concentration of B", "amount of a substance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles (pounds mass)/US gallon",
     {"concentration of B", "amount of a substance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles (pounds mass)/hour",
     {"mole per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles (pounds mass)/hour square foot",
     {"mole per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles (pounds mass)/second",
     {"mole per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"moles (pounds mass)/second square foot",
     {"mole per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millipascal",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"megapascals",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"millipascal seconds",
     {"dynamic viscosity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megapascal seconds/metre (megarayl)",
     {"mass per time per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megapascal per hour",
     {"pressure per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megapascal per meter",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"mega pounds per square inch",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     6.89476e+009,
     1,
     0},
    {"milliradian",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"megaradian",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"milli-rem",
     {"dose equivalent", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     133,
     RescueUOM::FACTOR,
     0,
     1e-005,
     1,
     0},
    {"milli-rems per hour",
     {"dose equivalent rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millisiemen",
     {"electric conductance", "admittance", "susceptance", "", "", "", "", "", "", "", "", "", ""},
     3,
     120,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"milliseconds",
     {"time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     121,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"half a millisecond",
     {"time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     121,
     RescueUOM::FACTOR,
     0,
     0.0005,
     1,
     0},
    {"milliseconds/centimetre",
     {"time per length", "slowness", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millisecond per foot",
     {"time per length", "slowness", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milliseconds/inch",
     {"time per length", "slowness", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millisiemens/metre",
     {"conductivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millisecond per meter",
     {"time per length", "slowness", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milliseconds/second",
     {"relative time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"thousand cubic feet at 60 deg F",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     128,
     RescueUOM::FACTOR,
     0,
     28.2624,
     1,
     0},
    {"thousand standard cubic feet/day",
     {"standard volume per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"thousand std cu ft/ stock tank barrel",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     28.2624,
     0.158987,
     0},
    {"thousand std cubic metres, 15 degC/day",
     {"standard volume per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milliseconds angular",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FACTOR,
     0,
     4.84814e-009,
     1,
     0},
    {"mega standard cubic metres 15C",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     128,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"thousand stock tank barrels 60 F",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     128,
     RescueUOM::FACTOR,
     0,
     158.987,
     1,
     0},
    {"thousand stock tank barrels,60 deg F/day",
     {"standard volume per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millisievert",
     {"dose equivalent", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     133,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"millisieverts per hour",
     {"dose equivalent rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milliteslas",
     {"magnetic induction", "magnetic flux density", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     135,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"megavolt",
     {"electric potential", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     136,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"millivolts",
     {"electric potential", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     136,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"millivolt per foot",
     {"electric field strength", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"millivolt per meter",
     {"electric field strength", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megawatts",
     {"power", "heat flow rate", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     139,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"milliwatt",
     {"power", "heat flow rate", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     139,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"megawatt hours",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     3.6e+009,
     1,
     0},
    {"megawatt hours/kilogram",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"megawatt hours/cubic metre",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milliwatts/square metres",
     {"density of heat flow rate", "poynting vector", "radiant energy", "fluence rate", "radiant exitance",
      "irradiance", "sound intensity", "energy fluence rate", "", "", "", "", ""},
     8,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"milliwebers",
     {"magnetic flux", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     148,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"newton metre",
     {"moment of force", "moment of couple", "torque", "", "", "", "", "", "", "", "", "", ""},
     3,
     41,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"newton metres/metre",
     {"force length per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"newton seconds/metre squared",
     {"dynamic viscosity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"newton per thirty meters",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"newtons/square metre",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"newtons/square millimetre",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     1e+006,
     1,
     0},
    {"nanoampere",
     {"electric current", "magnetic potential difference", "magnetomotive force", "", "", "", "", "", "", "", "", "",
      ""},
     3,
     10,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"nautical mile",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     1852,
     1,
     0},
    {"nanocoulomb",
     {"electric capacity", "electric charge", "electric flux", "", "", "", "", "", "", "", "", "", ""},
     3,
     22,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"nanocurie",
     {"activity (of radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     19,
     RescueUOM::FACTOR,
     0,
     37,
     1,
     0},
    {"nanoeuclid",
     {"dimensionless", "poisson ratio", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"nanohenry",
     {"self inductance", "permeance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     38,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"nanojoules",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"nanometres",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "diameter", "length of path",
      "wavelength", "mean free path", "", "", ""},
     10,
     69,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"nanometer per second",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"nanohm",
     {"resistance", "impedance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     103,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"nanoseconds",
     {"time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     121,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"nanoseconds/foot",
     {"time per length", "slowness", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"nanoseconds/metre",
     {"time per length", "slowness", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"nanoteslas",
     {"magnetic induction", "magnetic flux density", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     135,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"nanowatts",
     {"power", "heat flow rate", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     139,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"oersted",
     {"linear electric current density", "magnetic field strength", "magnetization", "", "", "", "", "", "", "", "", "",
      ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"ohm centimetres",
     {"electrical resistivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"avoirdupois ounces",
     {"mass", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     53,
     RescueUOM::FACTOR,
     0,
     0.0283495,
     1,
     0},
    {"troy ounces",
     {"mass", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     53,
     RescueUOM::FACTOR,
     0,
     0.0311035,
     1,
     0},
    {"ounce force",
     {"force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FRACTION,
     0,
     4.44822,
     16,
     0},
    {"ounce mass",
     {"mass", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     53,
     RescueUOM::FRACTION,
     0,
     0.453592,
     16,
     0},
    {"poise",
     {"dynamic viscosity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"picoampere",
     {"electric current", "magnetic potential difference", "magnetomotive force", "", "", "", "", "", "", "", "", "",
      ""},
     3,
     10,
     RescueUOM::FACTOR,
     0,
     1e-012,
     1,
     0},
    {"pascal seconds squared/ cubic metre",
     {"", "", "", "", "", "", "", "", "", "", "", "", ""},
     0,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pascal gauge",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::NONE,
     101325,
     1,
     1,
     0},
    {"pascal per hour",
     {"pressure per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"picocoulomb",
     {"electric capacity", "electric charge", "electric flux", "", "", "", "", "", "", "", "", "", ""},
     3,
     22,
     RescueUOM::FACTOR,
     0,
     1e-012,
     1,
     0},
    {"picocurie per gram",
     {"specific activity (of radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"picocurie",
     {"activity (of radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     19,
     RescueUOM::FACTOR,
     0,
     0.037,
     1,
     0},
    {"poundals",
     {"force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     0.138255,
     1,
     0},
    {"poundal centimetre squared",
     {"force area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"foot poundal",
     {"moment of force", "moment of couple", "torque", "", "", "", "", "", "", "", "", "", ""},
     3,
     41,
     RescueUOM::FACTOR,
     0,
     0.0421401,
     1,
     0},
    {"poundals/centimetre",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"per mille",
     {"volumic concentration", "mass concentration", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"picrofarads",
     {"capacitance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     33,
     RescueUOM::FACTOR,
     0,
     1e-012,
     1,
     0},
    {"picometer",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "diameter", "length of path",
      "wavelength", "mean free path", "", "", ""},
     10,
     69,
     RescueUOM::FACTOR,
     0,
     1e-012,
     1,
     0},
    {"picopascal",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     1e-012,
     1,
     0},
    {"parts per ten thousand",
     {"volumic concentration", "mass concentration", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     0.0001,
     1,
     0},
    {"parts per thousand",
     {"volumic concentration", "mass concentration", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"parts per million",
     {"volumic concentration", "mass concentration", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"part per million per degree Celsius",
     {"volumetric thermal expansion", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"part per million per degree Fahrenheit",
     {"volumetric thermal expansion", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"picosecond",
     {"time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     121,
     RescueUOM::FACTOR,
     0,
     1e-012,
     1,
     0},
    {"picosiemens",
     {"electric conductance", "admittance", "susceptance", "", "", "", "", "", "", "", "", "", ""},
     3,
     120,
     RescueUOM::FACTOR,
     0,
     1e-009,
     1,
     0},
    {"pounds/square foot",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     47.8803,
     1,
     0},
    {"pounds/square inch",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     6894.76,
     1,
     0},
    {"pounds per square inch days/barrel",
     {"pressure time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pound per square inch second",
     {"dynamic viscosity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds/square inch per 100 feet",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds/square inch per foot",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pound per square inch per hour",
     {"pressure per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds/square inch per thousand feet",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pound per square inch per meter",
     {"force per volume", "pressure per length", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pound per square inch per minute",
     {"pressure per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pound per square inch squared",
     {"pressure squared", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     113,
     RescueUOM::FACTOR,
     0,
     4.75377e+007,
     1,
     0},
    {"psi squared days/ centipoise cubic foot",
     {"Darcy flow coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"(psi days/cubic foot)squared/centipoise",
     {"nonDarcy flow coefficient", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds/square inch squared/ centipoise",
     {"pressure squared per (dynamic viscosity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"pounds/square inch absolute",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     6894.76,
     1,
     0},
    {"pounds/square inch gauge",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::NONE,
     101325,
     6894.76,
     1,
     0},
    {"UK pint",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     0.000568261,
     1,
     0},
    {"UK pints/horsepower hour",
     {"isothermal compressibility", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK pints/1000 barrels",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     0.000568261,
     158.987,
     0},
    {"US pints",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     0.000473177,
     1,
     0},
    {"US pint per ten barrel",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     0.473177,
     0.0158987,
     0},
    {"UK quarts",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     0.00113652,
     1,
     0},
    {"US quarts",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     0.000946353,
     1,
     0},
    {"quads",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1.05506e+018,
     1,
     0},
    {"quads/year",
     {"heat flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FRACTION,
     0,
     1.05506e+018,
     3.15582e+007,
     0},
    {"radians per foot",
     {"angle per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"radians per cubic foot",
     {"angle per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"rad", {"absorbed dose", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 37, RescueUOM::FACTOR, 0, 0.01, 1, 0},
    {"rem",
     {"dose equivalent", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     133,
     RescueUOM::FACTOR,
     0,
     0.01,
     1,
     0},
    {"rems per hour",
     {"dose equivalent rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"revolutions/minute",
     {"angle per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"revolutions/second",
     {"angle per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"revolutions/minute per second",
     {"angular acceleration", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"seconds/centimetre",
     {"time per length", "slowness", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"seconds/foot",
     {"time per length", "slowness", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"second per cubic foot",
     {"time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"seconds/inch",
     {"time per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"second per litre",
     {"time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"second per UK quart",
     {"time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"second per US quart",
     {"time per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"sacks", {"mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 53, RescueUOM::FACTOR, 0, 42.6377, 1, 0},
    {"std cubic feet at 60 deg F",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     128,
     RescueUOM::FACTOR,
     0,
     0.0282624,
     1,
     0},
    {"std cubic feet at 60 deg F/barrel",
     {"standard volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"standard cubic feet/day",
     {"standard volume per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"std cubic feet at 60 deg F/square foot",
     {"standard volume per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"std cubic feet at 60 deg Ft/cubic foot",
     {"standard volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"std cubic metres at 15 deg C/day",
     {"standard volume per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"std cubic metres / stock tank barrel",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     1,
     0.158987,
     0},
    {"seconds angular",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FACTOR,
     0,
     4.84814e-006,
     1,
     0},
    {"std cubic metres/ 1000 std cubic metre",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     1,
     1000,
     0},
    {"std cubic metres/ std cubic metres",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"square feet",
     {"area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     0.092903,
     1,
     0},
    {"square yards",
     {"area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     0.836127,
     1,
     0},
    {"stock tank barrel at 60 deg F",
     {"standard volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     128,
     RescueUOM::FACTOR,
     0,
     0.158987,
     1,
     0},
    {"stock tank barrels, 60 deg F/acre",
     {"standard volume per area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"stock tank barrels, 60 deg F/barrel",
     {"standard volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"stock tank barrels, 60 deg F/day",
     {"standard volume per time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"stock tank barrels/ million std cu ft",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     0.158987,
     28262.4,
     0},
    {"stock tank barrels/ million std cu mts",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     0.158987,
     1e+006,
     0},
    {"stock tank barrels/ 1000 std cu ft",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     0.158987,
     28.2624,
     0},
    {"stock tank barrels/ 1000 std cu metres",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FRACTION,
     0,
     0.158987,
     1000,
     0},
    {"stock tank barrels/ std cu metres",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.158987,
     1,
     0},
    {"sieverts per hour",
     {"dose equivalent rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"tonne", {"mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 53, RescueUOM::FACTOR, 0, 1000, 1, 0},
    {"tonnes/year",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"tonnes/day",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"tonnes/hour",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"tonnes per minute",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"talbot",
     {"quantity of light", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"terabecquerel",
     {"activity (of radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     19,
     RescueUOM::FACTOR,
     0,
     1e+012,
     1,
     0},
    {"trillion cubic feet",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     2.83169e+010,
     1,
     0},
    {"tera electron volts",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1.60218e-007,
     1,
     0},
    {"therms",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1.05506e+008,
     1,
     0},
    {"therms/cubic foot",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"therms/UK gallon",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"therms/pound mass",
     {"specific energy", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"terajoules",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1e+012,
     1,
     0},
    {"terajoules/year",
     {"heat flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FRACTION,
     0,
     1e+012,
     3.15582e+007,
     0},
    {"teraohm",
     {"resistance", "impedance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     103,
     RescueUOM::FACTOR,
     0,
     1e+012,
     1,
     0},
    {"tons of refrigeration",
     {"power", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     139,
     RescueUOM::FACTOR,
     0,
     3516.85,
     1,
     0},
    {"UK tons force",
     {"force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     9964.02,
     1,
     0},
    {"UK ton feet squared",
     {"force area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK tons force/foot",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK tons force/square foot",
     {"pressure", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     106,
     RescueUOM::FRACTION,
     0,
     9964.02,
     0.092903,
     0},
    {"US tons force",
     {"force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     8896.44,
     1,
     0},
    {"US tons force feet",
     {"moment of force", "moment of couple", "torque", "", "", "", "", "", "", "", "", "", ""},
     3,
     41,
     RescueUOM::FACTOR,
     0,
     2711.64,
     1,
     0},
    {"US tons force feet squared",
     {"force area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US tons force miles",
     {"moment of force", "moment of couple", "torque", "", "", "", "", "", "", "", "", "", ""},
     3,
     41,
     RescueUOM::FACTOR,
     0,
     1.43174e+007,
     1,
     0},
    {"US ton force miles/barrel",
     {"normal stress", "shear stress", "modulus of elasticity", "shear modulus", "modulus of rigidity", "bulk modulus",
      "modulus of compression", "electromagnetic energy density", "radiant energy density", "", "", "", ""},
     9,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US tons force miles/foot",
     {"force length per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FRACTION,
     0,
     1.43174e+007,
     0.3048,
     0},
    {"US tons force/foot",
     {"force per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US tons force/square foot",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     95760.5,
     1,
     0},
    {"US tons force/square inch",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     1.37895e+007,
     1,
     0},
    {"UK tons", {"mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 53, RescueUOM::FACTOR, 0, 1016.05, 1, 0},
    {"UK tons/year",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK tons/day",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK tons/hour",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"UK tons/minute",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US tons", {"mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 53, RescueUOM::FACTOR, 0, 907.185, 1, 0},
    {"US tons/year",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US tons/day",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US tons/square foot",
     {"surface density", "areic mass", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US tons/hour",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"US tons/minute",
     {"mass flow rate", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"torr",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     133.322,
     1,
     0},
    {"terawatts",
     {"power", "heat flow rate", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     139,
     RescueUOM::FACTOR,
     0,
     1e+012,
     1,
     0},
    {"terrawatt hours",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     3.6e+015,
     1,
     0},
    {"microampere",
     {"electric current", "magnetic potential difference", "magnetomotive force", "", "", "", "", "", "", "", "", "",
      ""},
     3,
     10,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microampere per square centimeter",
     {"current density", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"microampere per square inch",
     {"current density", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"microbars",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     0.1,
     1,
     0},
    {"microcoulomb",
     {"electric capacity", "electric charge", "electric flux", "", "", "", "", "", "", "", "", "", ""},
     3,
     22,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microcalories/second square centimetre",
     {"density of heat flow rate", "poynting vector", "radiant energy", "fluence rate", "radiant exitance",
      "irradiance", "sound intensity", "energy fluence rate", "", "", "", "", ""},
     8,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"microcurie",
     {"activity (of radioactivity)", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     19,
     RescueUOM::FACTOR,
     0,
     37000,
     1,
     0},
    {"microEuclids",
     {"dimensionless", "poisson ratio", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     32,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microfarads",
     {"capacitance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     33,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microfarads/metre",
     {"permittivity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"micrograms", {"mass", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 53, RescueUOM::FACTOR, 0, 1e-009, 1, 0},
    {"micrograms/cubic centimetre",
     {"density", "mass density", "volumic mass", "", "", "", "", "", "", "", "", "", ""},
     3,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"microhenry",
     {"self inductance", "permeance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     38,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microhenries/metre",
     {"magnetic permeability", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"microhertz", {"frequency", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, -1, RescueUOM::NONE, 0, 1, 1, 0},
    {"microjoules",
     {"energy", "work", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     41,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microns",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "diameter", "length of path",
      "wavelength", "mean free path", "", "", ""},
     10,
     69,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"micrometer per second",
     {"velocity", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"square microns",
     {"area", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     73,
     RescueUOM::FACTOR,
     0,
     1e-012,
     1,
     0},
    {"square micron metres",
     {"volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     78,
     RescueUOM::FACTOR,
     0,
     1e-012,
     1,
     0},
    {"microns of Mercury at 0 deg C",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     0.133322,
     1,
     0},
    {"micromole",
     {"amount of substance", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     91,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"micronewtons",
     {"force", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     96,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"unitless",
     {"dimensionless", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"microohm",
     {"resistance", "impedance", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     103,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microhm per foot",
     {"resistivity per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"microhm per meter",
     {"resistivity per length", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"micropascal",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"micropounds/square inch",
     {"force per area", "pressure", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     106,
     RescueUOM::FACTOR,
     0,
     0.00689476,
     1,
     0},
    {"microradian",
     {"plane angle", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     115,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microsiemens",
     {"electric conductance", "admittance", "susceptance", "", "", "", "", "", "", "", "", "", ""},
     3,
     120,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microsecond",
     {"time", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     121,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microseconds/foot",
     {"time per length", "slowness", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"microseconds/metre",
     {"time per length", "slowness", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"microteslas",
     {"magnetic induction", "magnetic flux density", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     135,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microvolts",
     {"electric potential", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     136,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microvolt per foot",
     {"electric field strength", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"microvolt per meter",
     {"electric field strength", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"microwatts",
     {"power", "heat flow rate", "", "", "", "", "", "", "", "", "", "", ""},
     2,
     139,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"microwatts/cubic metre",
     {"power per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"microwebers",
     {"magnetic flux", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     148,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"volts/decibel",
     {"potential difference per per power drop", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"volume percent",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.01,
     1,
     0},
    {"volume parts per million",
     {"volume per volume", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"watts/square centimetre",
     {"density of heat flow rate", "poynting vector", "radiant energy", "fluence rate", "radiant exitance",
      "irradiance", "sound intensity", "energy fluence rate", "", "", "", "", ""},
     8,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"watts/kilowatt",
     {"relative power", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.001,
     1,
     0},
    {"watts per square millimeter",
     {"density of heat flow rate", "poynting vector", "radiant energy", "fluence rate", "radiant exitance",
      "irradiance", "sound intensity", "energy fluence rate", "", "", "", "", ""},
     8,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"watts/watt",
     {"relative power", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1,
     1,
     0},
    {"webers/millimetre",
     {"magnetic vector potential", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     -1,
     RescueUOM::NONE,
     0,
     1,
     1,
     0},
    {"weeks", {"time", "", "", "", "", "", "", "", "", "", "", "", ""}, 1, 121, RescueUOM::FACTOR, 0, 604800, 1, 0},
    {"weight percent",
     {"mass concentration", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     0.01,
     1,
     0},
    {"weight parts per million",
     {"mass concentration", "", "", "", "", "", "", "", "", "", "", "", ""},
     1,
     32,
     RescueUOM::FACTOR,
     0,
     1e-006,
     1,
     0},
    {"yards",
     {"length", "breadth", "height", "thickness", "radius", "radius of curvature", "Cartesian coordinates", "diameter",
      "length of path", "distance", "", "", ""},
     10,
     69,
     RescueUOM::FACTOR,
     0,
     0.9144,
     1,
     0},
    {"Benoits yard (1895 A)",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     0.914399,
     1,
     0},
    {"Benoits yard (1895 B)",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     36,
     39.3701,
     0},
    {"Clarkes yard",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     0.914392,
     1,
     0},
    {"imperial yard",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     0.914392,
     1,
     0},
    {"Indian yard",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     36,
     39.3701,
     0},
    {"Indian yard (1937)",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     0.914395,
     1,
     0},
    {"Indian yard (1962)",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     0.914399,
     1,
     0},
    {"Indian yard (1975)",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FACTOR,
     0,
     0.914399,
     1,
     0},
    {"Sears yard",
     {"length", "depth", "Cartesian coordinates", "distance", "", "", "", "", "", "", "", "", ""},
     4,
     69,
     RescueUOM::FRACTION,
     0,
     36,
     39.3701,
     0}};

const RESCUECHAR *RescueUOM::Name(RESCUEINT32 zeroBasedNdx) {
  const RESCUECHAR *myReturn = 0;
  if (zeroBasedNdx >= 0 && zeroBasedNdx < UOMCount()) {
    myReturn = uom[zeroBasedNdx].name;
  }
  return myReturn;
}

RESCUEBOOL RescueUOM::IsBase(RESCUEINT32 zeroBasedNdx) {
  RESCUEBOOL myReturn = false;
  if (zeroBasedNdx >= 0 && zeroBasedNdx < UOMCount()) {
    if (uom[zeroBasedNdx].baseUnit == -1) {
      myReturn = true;
    }
  }
  return myReturn;
}

const RESCUECHAR **RescueUOM::QuantityTypeArray(RESCUEINT32 zeroBasedNdx) {
  const RESCUECHAR **myReturn = 0;
  if (zeroBasedNdx >= 0 && zeroBasedNdx < UOMCount()) {
    myReturn = uom[zeroBasedNdx].quantityTypeArray;
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::QuantityTypeCount(RESCUEINT32 zeroBasedNdx) {
  RESCUEINT32 myReturn = 0;
  if (zeroBasedNdx >= 0 && zeroBasedNdx < UOMCount()) {
    myReturn = uom[zeroBasedNdx].quantityTypeCount;
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::BaseUnitIndex(RESCUEINT32 zeroBasedNdx) {
  RESCUEINT32 myReturn = -1;
  if (zeroBasedNdx >= 0 && zeroBasedNdx < UOMCount()) {
    myReturn = uom[zeroBasedNdx].baseUnit;
  }
  return myReturn;
}

RescueUOM::ConversionTypes RescueUOM::ConversionType(RESCUEINT32 zeroBasedNdx) {
  RescueUOM::ConversionTypes myReturn = RescueUOM::NONE;
  if (zeroBasedNdx >= 0 && zeroBasedNdx < UOMCount()) {
    myReturn = uom[zeroBasedNdx].conversionType;
  }
  return myReturn;
}

RESCUEDOUBLE RescueUOM::ConversionFactor(RESCUEINT32 zeroBasedNdx) { return RescueUOM::Conversion_B(zeroBasedNdx); }

RESCUEDOUBLE RescueUOM::ConversionNumerator(RESCUEINT32 zeroBasedNdx) { return RescueUOM::Conversion_B(zeroBasedNdx); }

RESCUEDOUBLE RescueUOM::ConversionDenominator(RESCUEINT32 zeroBasedNdx) {
  return RescueUOM::Conversion_C(zeroBasedNdx);
}

RESCUEDOUBLE RescueUOM::Conversion_A(RESCUEINT32 zeroBasedNdx) {
  RESCUEDOUBLE myReturn = 0.0;
  if (zeroBasedNdx >= 0 && zeroBasedNdx < UOMCount()) {
    myReturn = uom[zeroBasedNdx].conversion_A;
  }
  return myReturn;
}

RESCUEDOUBLE RescueUOM::Conversion_B(RESCUEINT32 zeroBasedNdx) {
  RESCUEDOUBLE myReturn = 0.0;
  if (zeroBasedNdx >= 0 && zeroBasedNdx < UOMCount()) {
    myReturn = uom[zeroBasedNdx].conversion_B;
  }
  return myReturn;
}

RESCUEDOUBLE RescueUOM::Conversion_C(RESCUEINT32 zeroBasedNdx) {
  RESCUEDOUBLE myReturn = 0.0;
  if (zeroBasedNdx >= 0 && zeroBasedNdx < UOMCount()) {
    myReturn = uom[zeroBasedNdx].conversion_C;
  }
  return myReturn;
}

RESCUEDOUBLE RescueUOM::Conversion_D(RESCUEINT32 zeroBasedNdx) {
  RESCUEDOUBLE myReturn = 0.0;
  if (zeroBasedNdx >= 0 && zeroBasedNdx < UOMCount()) {
    myReturn = uom[zeroBasedNdx].conversion_D;
  }
  return myReturn;
}

RESCUEBOOL RescueUOM::IsPer(const RESCUECHAR *pos) {
  bool myReturn = false;
  if (*pos == ' ') {
    if (tolower(*++pos) == 'p') {
      if (tolower(*++pos) == 'e') {
        if (tolower(*++pos) == 'r') {
          if (*++pos == ' ') {
            myReturn = true;
          }
        }
      }
    }
  } else if (tolower(*pos) == 'p') {
    if (tolower(*++pos) == 'e') {
      if (tolower(*++pos) == 'r') {
        myReturn = true;
      }
    }
  }
  return myReturn;
}
RESCUEBOOL RescueUOM::IsIES(const RESCUECHAR *pos) {
  bool myReturn = false;
  if (tolower(*pos) == 'i') {
    if (tolower(*++pos) == 'e') {
      if (tolower(*++pos) == 's') {
        myReturn = true;
      }
    }
  }
  return myReturn;
}

int RescueUOM::CompareUOM(const RESCUECHAR *name1, const RESCUECHAR *name2) {
  /*
  Not a replacement for strcmp in a lexical sense.
  */
  int myReturn = 0;
  const RESCUECHAR *n1c = name1;
  const RESCUECHAR *n1n = n1c;
  n1n++;
  const RESCUECHAR *n2c = name2;
  const RESCUECHAR *n2n = n2c;
  n2n++;
  while (myReturn == 0 && *n1c != 0 && *n2c != 0) {
    if (*n1c == *n2c) {
      n1c++;
      n1n++;
      n2c++;
      n2n++;
    } else if (tolower(*n1c) == tolower(*n2c)) {
      n1c++;
      n1n++;
      n2c++;
      n2n++;
    } else if (tolower(*n1c) == tolower(*n2n) && tolower(*n1n) == tolower(*n2c)) {
      if ((tolower(*n1c) == 'r' || tolower(*n1c) == 'e') && (tolower(*n1n) == 'r' || tolower(*n1n) == 'e')) {
        n1c += 2;
        n1n += 2;
        n2c += 2;
        n2n += 2;
      } else {
        myReturn = -1;
      }
    } else if (*n1n == 0 || *n1n == ' ') {
      if (tolower(*n1c) == 's') {
        if (*n1n == *n2c) {
          n1c++;
          n1n++;
        } else {
          myReturn = -1;
        }
      } else if (tolower(*n1c) == 'y') {
        if (IsIES(n2c)) {
          n1c++;
          n1n++;
          n2c += 3;
          n2n += 3;
        } else {
          myReturn = -1;
        }
      } else if (tolower(*n1c) == 'i') {
        if (tolower(*n2c) == 'y') {
          if (IsIES(n1c)) {
            n1c += 3;
            n1n += 3;
            n2c++;
            n2n++;
          } else {
            myReturn = -1;
          }
        } else {
          myReturn = -1;
        }
      } else if (*n1c == '/' && IsPer(n2c)) {
        n1c++;
        n1n++;
        n2c += 3;
        n2n += 3;
      } else {
        myReturn = -1;
      }
    } else if (*n2n == 0 || *n2n == ' ') {
      if (tolower(*n2c) == 's') {
        if (*n2n == *n1c) {
          n2c++;
          n2n++;
        } else {
          myReturn = -1;
        }
      } else if (tolower(*n2c) == 'y') {
        if (IsIES(n1c)) {
          n2c++;
          n2n++;
          n1c += 3;
          n1n += 3;
        } else {
          myReturn = -1;
        }
      } else if (tolower(*n2c) == 'i') {
        if (tolower(*n1c) == 'y') {
          if (IsIES(n2c)) {
            n2c += 3;
            n2n += 3;
            n1c++;
            n1n++;
          } else {
            myReturn = -1;
          }
        } else {
          myReturn = -1;
        }
      } else if (*n2c == '/' && IsPer(n1c)) {
        n2c++;
        n2n++;
        n1c += 3;
        n1n += 3;
      } else {
        myReturn = -1;
      }
    } else if (*n2c == ' ' && *n1c == '/') {
      if (IsPer(n2c)) {
        n2c += 5;
        n2n += 5;
        n1c++;
        n1n++;
      } else {
        myReturn = -1;
      }
    } else if (*n2c == '/' && *n1c == ' ') {
      if (IsPer(n1c)) {
        n2c++;
        n2n++;
        n1c += 5;
        n1n += 5;
      } else {
        myReturn = -1;
      }
    } else if (*n1c == ' ' && *n2c == '/') {
      if (IsPer(n1c)) {
        n1c += 5;
        n1n += 5;
        n2c++;
        n2n++;
      } else {
        myReturn = -1;
      }
    } else if (*n1c == '/' && *n2c == ' ') {
      if (IsPer(n2c)) {
        n1c++;
        n1n++;
        n2c += 5;
        n2n += 5;
      } else {
        myReturn = -1;
      }
    } else {
      myReturn = -1;
    }
  }
  if (*n1c != 0) {
    if (tolower(*n1c) != 's' || *n1n != 0) {
      myReturn = -1;
    }
  }
  if (myReturn == 0 && *n2c != 0) {
    if (tolower(*n2c) != 's' || *n2n != 0) {
      myReturn = -1;
    }
  }
  return myReturn;
}

int RescueUOM::IndexFromName(const RESCUECHAR *name) {
  int myReturn = -1;
  if (name != 0) {
    switch (tolower(name[0])) {
    case '%':
      if (CompareUOM(name, "%") == 0) {
        myReturn = 1218;
      }
      break;
    case '(':
      if (CompareUOM(name, "(cubic metres per hour) per kilopascal") == 0) {
        myReturn = 829;
      } else if (CompareUOM(name, "(cubic metres/second) per pascal") == 0) {
        myReturn = 83;
      } else if (CompareUOM(name, "(dyne/cm)4/gcm3") == 0) {
        myReturn = 411;
      } else if (CompareUOM(name, "(hydraulic) horsepower per square inch") == 0) {
        myReturn = 579;
      } else if (CompareUOM(name, "(l/min)/bar") == 0) {
        myReturn = 724;
      } else if (CompareUOM(name, "(m3/h)/kpa") == 0) {
        myReturn = 829;
      } else if (CompareUOM(name, "(n/m)4/kg.m3") == 0) {
        myReturn = 412;
      } else if (CompareUOM(name, "(psi days/cubic foot)squared/centipoise") == 0) {
        myReturn = 1073;
      }
      break;
    case '.':
      if (CompareUOM(name, ".1 pa") == 0) {
        myReturn = 410;
      }
      break;
    case '1':
      myReturn = IndexFromName_1(name);
      break;
    case '3':
      if (CompareUOM(name, "32nd of an inch") == 0) {
        myReturn = 591;
      }
      break;
    case '6':
      if (CompareUOM(name, "64th of an inch") == 0) {
        myReturn = 592;
      }
      break;
    case 'a':
      myReturn = IndexFromName_a(name);
      break;
    case 'b':
      myReturn = IndexFromName_b(name);
      break;
    case 'c':
      myReturn = IndexFromName_c(name);
      break;
    case 'd':
      myReturn = IndexFromName_d(name);
      break;
    case 'e':
      myReturn = IndexFromName_e(name);
      break;
    case 'f':
      myReturn = IndexFromName_f(name);
      break;
    case 'g':
      myReturn = IndexFromName_g(name);
      break;
    case 'h':
      myReturn = IndexFromName_h(name);
      break;
    case 'i':
      myReturn = IndexFromName_i(name);
      break;
    case 'j':
      myReturn = IndexFromName_j(name);
      break;
    case 'k':
      myReturn = IndexFromName_k(name);
      break;
    case 'l':
      myReturn = IndexFromName_l(name);
      break;
    case 'm':
      myReturn = IndexFromName_m(name);
      break;
    case 'n':
      myReturn = IndexFromName_n(name);
      break;
    case 'o':
      myReturn = IndexFromName_o(name);
      break;
    case 'p':
      myReturn = IndexFromName_p(name);
      break;
    case 'q':
      if (CompareUOM(name, "quads") == 0) {
        myReturn = 1084;
      } else if (CompareUOM(name, "quads/year") == 0) {
        myReturn = 1085;
      }
      break;
    case 'r':
      myReturn = IndexFromName_r(name);
      break;
    case 's':
      myReturn = IndexFromName_s(name);
      break;
    case 't':
      myReturn = IndexFromName_t(name);
      break;
    case 'u':
      myReturn = IndexFromName_u(name);
      break;
    case 'v':
      if (CompareUOM(name, "v") == 0) {
        myReturn = 136;
      } else if (CompareUOM(name, "v/b") == 0) {
        myReturn = 137;
      } else if (CompareUOM(name, "v/db") == 0) {
        myReturn = 1209;
      } else if (CompareUOM(name, "v/m") == 0) {
        myReturn = 138;
      } else if (CompareUOM(name, "volt") == 0) {
        myReturn = 136;
      } else if (CompareUOM(name, "volts/bel") == 0) {
        myReturn = 137;
      } else if (CompareUOM(name, "volts/decibel") == 0) {
        myReturn = 1209;
      } else if (CompareUOM(name, "volts/metre") == 0) {
        myReturn = 138;
      } else if (CompareUOM(name, "volume parts per million") == 0) {
        myReturn = 1211;
      } else if (CompareUOM(name, "volume percent") == 0) {
        myReturn = 1210;
      }
      break;
    case 'w':
      myReturn = IndexFromName_w(name);
      break;
    case 'y':
      if (CompareUOM(name, "yards") == 0) {
        myReturn = 1220;
      } else if (CompareUOM(name, "yd2") == 0) {
        myReturn = 1113;
      } else if (CompareUOM(name, "yd3") == 0) {
        myReturn = 339;
      } else if (CompareUOM(name, "yr(100k)") == 0) {
        myReturn = 195;
      }
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_1(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "1/16 in") == 0) {
    myReturn = 590;
  } else if (CompareUOM(name, "1/27 ft3") == 0) {
    myReturn = 339;
  } else if (CompareUOM(name, "1/32 in") == 0) {
    myReturn = 591;
  } else if (CompareUOM(name, "1/64 in") == 0) {
    myReturn = 592;
  } else if (CompareUOM(name, "1/a") == 0) {
    myReturn = 152;
  } else if (CompareUOM(name, "1/bar") == 0) {
    myReturn = 154;
  } else if (CompareUOM(name, "1/bbl") == 0) {
    myReturn = 155;
  } else if (CompareUOM(name, "1/cm") == 0) {
    myReturn = 156;
  } else if (CompareUOM(name, "1/d") == 0) {
    myReturn = 157;
  } else if (CompareUOM(name, "1/degc") == 0) {
    myReturn = 158;
  } else if (CompareUOM(name, "1/degf") == 0) {
    myReturn = 159;
  } else if (CompareUOM(name, "1/degr") == 0) {
    myReturn = 160;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "1/ft") == 0) {
      myReturn = 161;
    } else if (CompareUOM(name, "1/ft2") == 0) {
      myReturn = 162;
    } else if (CompareUOM(name, "1/ft3") == 0) {
      myReturn = 163;
    } else if (CompareUOM(name, "1/g") == 0) {
      myReturn = 164;
    } else if (CompareUOM(name, "1/galuk") == 0) {
      myReturn = 165;
    } else if (CompareUOM(name, "1/galus") == 0) {
      myReturn = 166;
    } else if (CompareUOM(name, "1/h") == 0) {
      myReturn = 167;
    } else if (CompareUOM(name, "1/in") == 0) {
      myReturn = 168;
    } else if (CompareUOM(name, "1/k") == 0) {
      myReturn = 1;
    } else if (CompareUOM(name, "1/kg") == 0) {
      myReturn = 2;
    } else if (CompareUOM(name, "1/km2") == 0) {
      myReturn = 169;
    } else if (CompareUOM(name, "1/kpa") == 0) {
      myReturn = 170;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "1/l") == 0) {
      myReturn = 171;
    } else if (CompareUOM(name, "1/lbf") == 0) {
      myReturn = 172;
    } else if (CompareUOM(name, "1/lbm") == 0) {
      myReturn = 173;
    } else if (CompareUOM(name, "1/m") == 0) {
      myReturn = 3;
    } else if (CompareUOM(name, "1/m2") == 0) {
      myReturn = 4;
    } else if (CompareUOM(name, "1/m3") == 0) {
      myReturn = 5;
    } else if (CompareUOM(name, "1/mi") == 0) {
      myReturn = 174;
    } else if (CompareUOM(name, "1/mi2") == 0) {
      myReturn = 175;
    } else if (CompareUOM(name, "1/min") == 0) {
      myReturn = 176;
    } else if (CompareUOM(name, "1/mm") == 0) {
      myReturn = 177;
    } else if (CompareUOM(name, "1/n") == 0) {
      myReturn = 6;
    } else if (CompareUOM(name, "1/nm") == 0) {
      myReturn = 178;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "1/pa") == 0) {
      myReturn = 7;
    } else if (CompareUOM(name, "1/ppa") == 0) {
      myReturn = 179;
    } else if (CompareUOM(name, "1/psi") == 0) {
      myReturn = 180;
    } else if (CompareUOM(name, "1/s") == 0) {
      myReturn = 429;
    } else if (CompareUOM(name, "1/upsi") == 0) {
      myReturn = 181;
    } else if (CompareUOM(name, "1/uv") == 0) {
      myReturn = 182;
    } else if (CompareUOM(name, "1/v") == 0) {
      myReturn = 9;
    } else if (CompareUOM(name, "10 db") == 0) {
      myReturn = 14;
    } else if (CompareUOM(name, "10 db/m") == 0) {
      myReturn = 15;
    } else if (CompareUOM(name, "100%") == 0) {
      myReturn = 32;
    } else if (CompareUOM(name, "1000 std cubic metres/ std cubic metre") == 0) {
      myReturn = 707;
    } else if (CompareUOM(name, "100000 years") == 0) {
      myReturn = 195;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "16th of an inch") == 0) {
      myReturn = 590;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_a(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "a") == 0) {
    myReturn = 197;
  } else if (CompareUOM(name, "a.h") == 0) {
    myReturn = 198;
  } else if (CompareUOM(name, "a.m2") == 0) {
    myReturn = 11;
  } else if (CompareUOM(name, "a/cm2") == 0) {
    myReturn = 199;
  } else if (CompareUOM(name, "a/ft2") == 0) {
    myReturn = 200;
  } else if (CompareUOM(name, "a/m") == 0) {
    myReturn = 12;
  } else if (CompareUOM(name, "a/m2") == 0) {
    myReturn = 13;
  } else if (CompareUOM(name, "a/mm") == 0) {
    myReturn = 201;
  } else if (CompareUOM(name, "a/mm2") == 0) {
    myReturn = 202;
  } else if (CompareUOM(name, "acre") == 0) {
    myReturn = 203;
  } else if (CompareUOM(name, "acre feet/million stbs, 60 deg f") == 0) {
    myReturn = 205;
  } else if (CompareUOM(name, "acre foot") == 0) {
    myReturn = 204;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "acre.ft") == 0) {
      myReturn = 204;
    } else if (CompareUOM(name, "ag") == 0) {
      myReturn = 206;
    } else if (CompareUOM(name, "aj") == 0) {
      myReturn = 207;
    } else if (CompareUOM(name, "ampere") == 0) {
      myReturn = 10;
    } else if (CompareUOM(name, "ampere hour") == 0) {
      myReturn = 198;
    } else if (CompareUOM(name, "ampere per square centimeter") == 0) {
      myReturn = 199;
    } else if (CompareUOM(name, "ampere per square foot") == 0) {
      myReturn = 200;
    } else if (CompareUOM(name, "ampere/millimetre") == 0) {
      myReturn = 201;
    } else if (CompareUOM(name, "ampere/square millimetre") == 0) {
      myReturn = 202;
    } else if (CompareUOM(name, "amperes metres squared") == 0) {
      myReturn = 11;
    } else if (CompareUOM(name, "amperes/metre") == 0) {
      myReturn = 12;
    } else if (CompareUOM(name, "amperes/square metre") == 0) {
      myReturn = 13;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "angstrom") == 0) {
      myReturn = 208;
    } else if (CompareUOM(name, "annum") == 0) {
      myReturn = 197;
    } else if (CompareUOM(name, "api gamma ray units") == 0) {
      myReturn = 36;
    } else if (CompareUOM(name, "api gravity") == 0) {
      myReturn = 355;
    } else if (CompareUOM(name, "api neutron units") == 0) {
      myReturn = 101;
    } else if (CompareUOM(name, "atm") == 0) {
      myReturn = 210;
    } else if (CompareUOM(name, "atm/ft") == 0) {
      myReturn = 211;
    } else if (CompareUOM(name, "atm/h") == 0) {
      myReturn = 212;
    } else if (CompareUOM(name, "atm/m") == 0) {
      myReturn = 214;
    } else if (CompareUOM(name, "atmosphere") == 0) {
      myReturn = 210;
    } else if (CompareUOM(name, "atmosphere per hour") == 0) {
      myReturn = 212;
    } else if (CompareUOM(name, "atmospheres per ft") == 0) {
      myReturn = 211;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "atmospheres per hundred metre") == 0) {
      myReturn = 213;
    } else if (CompareUOM(name, "atmospheres/metre") == 0) {
      myReturn = 214;
    } else if (CompareUOM(name, "attogram") == 0) {
      myReturn = 206;
    } else if (CompareUOM(name, "attojoule") == 0) {
      myReturn = 207;
    } else if (CompareUOM(name, "avoirdupois ounces") == 0) {
      myReturn = 1034;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_b(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "b") == 0) {
    myReturn = 215;
  } else if (CompareUOM(name, "b/cm3") == 0) {
    myReturn = 216;
  } else if (CompareUOM(name, "bar") == 0) {
    myReturn = 218;
  } else if (CompareUOM(name, "bar per hour") == 0) {
    myReturn = 219;
  } else if (CompareUOM(name, "bar per kilometer") == 0) {
    myReturn = 220;
  } else if (CompareUOM(name, "bar per meter") == 0) {
    myReturn = 221;
  } else if (CompareUOM(name, "bar squared") == 0) {
    myReturn = 222;
  } else if (CompareUOM(name, "bar squared per centipoise") == 0) {
    myReturn = 223;
  } else if (CompareUOM(name, "bar/h") == 0) {
    myReturn = 219;
  } else if (CompareUOM(name, "bar/km") == 0) {
    myReturn = 220;
  } else if (CompareUOM(name, "bar/m") == 0) {
    myReturn = 221;
  } else if (CompareUOM(name, "bar2") == 0) {
    myReturn = 222;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "bar2/cp") == 0) {
      myReturn = 223;
    } else if (CompareUOM(name, "barn") == 0) {
      myReturn = 215;
    } else if (CompareUOM(name, "barns/cubic centimetre") == 0) {
      myReturn = 216;
    } else if (CompareUOM(name, "barns/electron") == 0) {
      myReturn = 217;
    } else if (CompareUOM(name, "barrel") == 0) {
      myReturn = 224;
    } else if (CompareUOM(name, "barrel per cubic foot") == 0) {
      myReturn = 237;
    } else if (CompareUOM(name, "barrel per day per kilopascal") == 0) {
      myReturn = 242;
    } else if (CompareUOM(name, "barrel per day per psi") == 0) {
      myReturn = 247;
    } else if (CompareUOM(name, "barrel per hundred barrel") == 0) {
      myReturn = 225;
    } else if (CompareUOM(name, "barrel per million cubic feet") == 0) {
      myReturn = 243;
    } else if (CompareUOM(name, "barrel per minute") == 0) {
      myReturn = 245;
    } else if (CompareUOM(name, "barrel per thousand cubic feet") == 0) {
      myReturn = 241;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "barrel per u.k. ton") == 0) {
      myReturn = 249;
    } else if (CompareUOM(name, "barrel per u.s. ton") == 0) {
      myReturn = 250;
    } else if (CompareUOM(name, "barrel/acre foot") == 0) {
      myReturn = 227;
    } else if (CompareUOM(name, "barrel/barrel") == 0) {
      myReturn = 228;
    } else if (CompareUOM(name, "barrel/day") == 0) {
      myReturn = 230;
    } else if (CompareUOM(name, "barrel/day pounds/square inch") == 0) {
      myReturn = 234;
    } else if (CompareUOM(name, "barrel/foot") == 0) {
      myReturn = 236;
    } else if (CompareUOM(name, "barrel/hour") == 0) {
      myReturn = 238;
    } else if (CompareUOM(name, "barrel/inch") == 0) {
      myReturn = 240;
    } else if (CompareUOM(name, "barrel/mile") == 0) {
      myReturn = 244;
    } else if (CompareUOM(name, "barrels/acre") == 0) {
      myReturn = 226;
    } else if (CompareUOM(name, "barrels/centipoise day psi") == 0) {
      myReturn = 229;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "barrels/day acre foot") == 0) {
      myReturn = 231;
    } else if (CompareUOM(name, "barrels/day foot") == 0) {
      myReturn = 232;
    } else if (CompareUOM(name, "barrels/day foot pounds/sq in") == 0) {
      myReturn = 233;
    } else if (CompareUOM(name, "barrels/day per day") == 0) {
      myReturn = 235;
    } else if (CompareUOM(name, "barrels/hour/hour") == 0) {
      myReturn = 239;
    } else if (CompareUOM(name, "barrels/million std cubic feet, 60 degf") == 0) {
      myReturn = 246;
    } else if (CompareUOM(name, "barrels/stock tank barrel, 60 deg f") == 0) {
      myReturn = 248;
    } else if (CompareUOM(name, "baud") == 0) {
      myReturn = 17;
    } else if (CompareUOM(name, "bbl") == 0) {
      myReturn = 224;
    } else if (CompareUOM(name, "bbl/(d.ft)") == 0) {
      myReturn = 232;
    } else if (CompareUOM(name, "bbl/(d.kpa)") == 0) {
      myReturn = 242;
    } else if (CompareUOM(name, "bbl/(d.psi)") == 0) {
      myReturn = 247;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "bbl/acre") == 0) {
      myReturn = 226;
    } else if (CompareUOM(name, "bbl/acre.ft") == 0) {
      myReturn = 227;
    } else if (CompareUOM(name, "bbl/bbl") == 0) {
      myReturn = 228;
    } else if (CompareUOM(name, "bbl/cp.d.psi") == 0) {
      myReturn = 229;
    } else if (CompareUOM(name, "bbl/d") == 0) {
      myReturn = 230;
    } else if (CompareUOM(name, "bbl/d.acre.ft") == 0) {
      myReturn = 231;
    } else if (CompareUOM(name, "bbl/d.ft") == 0) {
      myReturn = 232;
    } else if (CompareUOM(name, "bbl/d.ft.psi") == 0) {
      myReturn = 233;
    } else if (CompareUOM(name, "bbl/d.psi") == 0) {
      myReturn = 247;
    } else if (CompareUOM(name, "bbl/d2") == 0) {
      myReturn = 235;
    } else if (CompareUOM(name, "bbl/ft") == 0) {
      myReturn = 236;
    } else if (CompareUOM(name, "bbl/ft3") == 0) {
      myReturn = 237;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "bbl/hr") == 0) {
      myReturn = 238;
    } else if (CompareUOM(name, "bbl/hr2") == 0) {
      myReturn = 239;
    } else if (CompareUOM(name, "bbl/in") == 0) {
      myReturn = 240;
    } else if (CompareUOM(name, "bbl/kpa.d") == 0) {
      myReturn = 242;
    } else if (CompareUOM(name, "bbl/mi") == 0) {
      myReturn = 244;
    } else if (CompareUOM(name, "bbl/min") == 0) {
      myReturn = 245;
    } else if (CompareUOM(name, "bbl/psi.d") == 0) {
      myReturn = 247;
    } else if (CompareUOM(name, "bbl/tonuk") == 0) {
      myReturn = 249;
    } else if (CompareUOM(name, "bbl/tonus") == 0) {
      myReturn = 250;
    } else if (CompareUOM(name, "bd") == 0) {
      myReturn = 18;
    } else if (CompareUOM(name, "becquerel") == 0) {
      myReturn = 19;
    } else if (CompareUOM(name, "becquerel per kilogram") == 0) {
      myReturn = 20;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "bel") == 0) {
      myReturn = 14;
    } else if (CompareUOM(name, "bels/metre") == 0) {
      myReturn = 15;
    } else if (CompareUOM(name, "bels/octave") == 0) {
      myReturn = 16;
    } else if (CompareUOM(name, "benoit chain (1895 a)") == 0) {
      myReturn = 308;
    } else if (CompareUOM(name, "benoit chain (1895 b)") == 0) {
      myReturn = 309;
    } else if (CompareUOM(name, "benoits yard (1895 a)") == 0) {
      myReturn = 1221;
    } else if (CompareUOM(name, "benoits yard (1895 b)") == 0) {
      myReturn = 1222;
    } else if (CompareUOM(name, "billion cubic feet") == 0) {
      myReturn = 251;
    } else if (CompareUOM(name, "billions of electron volts") == 0) {
      myReturn = 550;
    } else if (CompareUOM(name, "bit") == 0) {
      myReturn = 252;
    } else if (CompareUOM(name, "bits per second") == 0) {
      myReturn = 18;
    } else if (CompareUOM(name, "bq") == 0) {
      myReturn = 19;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "bq/kg") == 0) {
      myReturn = 20;
    } else if (CompareUOM(name, "british foot (benoit 1895 a)") == 0) {
      myReturn = 481;
    } else if (CompareUOM(name, "british foot (benoit 1895 b)") == 0) {
      myReturn = 482;
    } else if (CompareUOM(name, "british foot 1865") == 0) {
      myReturn = 483;
    } else if (CompareUOM(name, "british link 1895 a") == 0) {
      myReturn = 785;
    } else if (CompareUOM(name, "british link 1895 b") == 0) {
      myReturn = 786;
    } else if (CompareUOM(name, "british thermal unit") == 0) {
      myReturn = 253;
    } else if (CompareUOM(name, "british thermal unit/hour") == 0) {
      myReturn = 261;
    } else if (CompareUOM(name, "british thermal units/barrel") == 0) {
      myReturn = 256;
    } else if (CompareUOM(name, "british thermal units/cubic foot") == 0) {
      myReturn = 258;
    } else if (CompareUOM(name, "british thermal units/hour cubic foot") == 0) {
      myReturn = 266;
    } else if (CompareUOM(name, "british thermal units/hour foot deg f") == 0) {
      myReturn = 262;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "british thermal units/minute") == 0) {
      myReturn = 272;
    } else if (CompareUOM(name, "british thermal units/pound mass") == 0) {
      myReturn = 269;
    } else if (CompareUOM(name, "british thermal units/pound mass deg f") == 0) {
      myReturn = 270;
    } else if (CompareUOM(name, "british thermal units/pound mass deg r") == 0) {
      myReturn = 271;
    } else if (CompareUOM(name, "british thermal units/pound mass mol") == 0) {
      myReturn = 273;
    } else if (CompareUOM(name, "british thermal units/second") == 0) {
      myReturn = 275;
    } else if (CompareUOM(name, "british thermal units/second square foot") == 0) {
      myReturn = 276;
    } else if (CompareUOM(name, "british thermal units/u.k. gallon") == 0) {
      myReturn = 259;
    } else if (CompareUOM(name, "british thermal units/u.s. gallon") == 0) {
      myReturn = 260;
    } else if (CompareUOM(name, "btu") == 0) {
      myReturn = 253;
    } else if (CompareUOM(name, "btu.in/h.ft2.degf") == 0) {
      myReturn = 254;
    } else if (CompareUOM(name, "btu/bbl") == 0) {
      myReturn = 256;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "btu/ft3") == 0) {
      myReturn = 258;
    } else if (CompareUOM(name, "btu/galuk") == 0) {
      myReturn = 259;
    } else if (CompareUOM(name, "btu/galus") == 0) {
      myReturn = 260;
    } else if (CompareUOM(name, "btu/h") == 0) {
      myReturn = 261;
    } else if (CompareUOM(name, "btu/h.ft.degf") == 0) {
      myReturn = 262;
    } else if (CompareUOM(name, "btu/h.ft2") == 0) {
      myReturn = 263;
    } else if (CompareUOM(name, "btu/h.ft2.degf") == 0) {
      myReturn = 264;
    } else if (CompareUOM(name, "btu/h.ft3") == 0) {
      myReturn = 266;
    } else if (CompareUOM(name, "btu/h.ft3.degf") == 0) {
      myReturn = 267;
    } else if (CompareUOM(name, "btu/h.m2.degc") == 0) {
      myReturn = 268;
    } else if (CompareUOM(name, "btu/hr.ft2.degr") == 0) {
      myReturn = 265;
    } else if (CompareUOM(name, "btu/lbm") == 0) {
      myReturn = 269;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "btu/lbm.degf") == 0) {
      myReturn = 270;
    } else if (CompareUOM(name, "btu/lbm.degr") == 0) {
      myReturn = 271;
    } else if (CompareUOM(name, "btu/min") == 0) {
      myReturn = 272;
    } else if (CompareUOM(name, "btu/s") == 0) {
      myReturn = 275;
    } else if (CompareUOM(name, "btu/s.ft2") == 0) {
      myReturn = 276;
    } else if (CompareUOM(name, "btu/s.ft2.degf") == 0) {
      myReturn = 277;
    } else if (CompareUOM(name, "btu/s.ft3") == 0) {
      myReturn = 278;
    } else if (CompareUOM(name, "btu/s.ft3.degf") == 0) {
      myReturn = 279;
    } else if (CompareUOM(name, "btus/brake-horsepower hour") == 0) {
      myReturn = 257;
    } else if (CompareUOM(name, "btus/hour foot cubed deg f") == 0) {
      myReturn = 267;
    } else if (CompareUOM(name, "btus/hour foot squared deg f") == 0) {
      myReturn = 264;
    } else if (CompareUOM(name, "btus/hour foot squared deg f per inch") == 0) {
      myReturn = 254;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "btus/hour foot squared deg r") == 0) {
      myReturn = 265;
    } else if (CompareUOM(name, "btus/hour metre squared deg c") == 0) {
      myReturn = 268;
    } else if (CompareUOM(name, "btus/hour per square foot") == 0) {
      myReturn = 263;
    } else if (CompareUOM(name, "btus/pound mass mol deg f") == 0) {
      myReturn = 274;
    } else if (CompareUOM(name, "btus/second per cubic foot") == 0) {
      myReturn = 278;
    } else if (CompareUOM(name, "btus/second per cubic foot deg f") == 0) {
      myReturn = 279;
    } else if (CompareUOM(name, "btus/second per square foot deg f") == 0) {
      myReturn = 277;
    } else if (CompareUOM(name, "byte") == 0) {
      myReturn = 21;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_c(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "c") == 0) {
    myReturn = 280;
  } else if (CompareUOM(name, "c.m") == 0) {
    myReturn = 23;
  } else if (CompareUOM(name, "c/cm2") == 0) {
    myReturn = 281;
  } else if (CompareUOM(name, "c/cm3") == 0) {
    myReturn = 282;
  } else if (CompareUOM(name, "c/g") == 0) {
    myReturn = 283;
  } else if (CompareUOM(name, "c/kg") == 0) {
    myReturn = 24;
  } else if (CompareUOM(name, "c/m2") == 0) {
    myReturn = 25;
  } else if (CompareUOM(name, "c/m3") == 0) {
    myReturn = 26;
  } else if (CompareUOM(name, "c/mm2") == 0) {
    myReturn = 284;
  } else if (CompareUOM(name, "c/mm3") == 0) {
    myReturn = 285;
  } else if (CompareUOM(name, "c/s") == 0) {
    myReturn = 1092;
  } else if (CompareUOM(name, "cal") == 0) {
    myReturn = 287;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "cal/cm3") == 0) {
      myReturn = 288;
    } else if (CompareUOM(name, "cal/g") == 0) {
      myReturn = 289;
    } else if (CompareUOM(name, "cal/g.k") == 0) {
      myReturn = 290;
    } else if (CompareUOM(name, "cal/h.cm.degc") == 0) {
      myReturn = 291;
    } else if (CompareUOM(name, "cal/h.cm2") == 0) {
      myReturn = 292;
    } else if (CompareUOM(name, "cal/h.cm2.degc") == 0) {
      myReturn = 293;
    } else if (CompareUOM(name, "cal/h.cm3") == 0) {
      myReturn = 294;
    } else if (CompareUOM(name, "cal/kg") == 0) {
      myReturn = 295;
    } else if (CompareUOM(name, "cal/lbm") == 0) {
      myReturn = 296;
    } else if (CompareUOM(name, "cal/ml") == 0) {
      myReturn = 297;
    } else if (CompareUOM(name, "cal/mm3") == 0) {
      myReturn = 298;
    } else if (CompareUOM(name, "cal/s.cm.degc") == 0) {
      myReturn = 300;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "cal/s.cm2.degc") == 0) {
      myReturn = 301;
    } else if (CompareUOM(name, "cal/s.cm3") == 0) {
      myReturn = 302;
    } else if (CompareUOM(name, "calorie") == 0) {
      myReturn = 287;
    } else if (CompareUOM(name, "calories/cubic centimetre") == 0) {
      myReturn = 288;
    } else if (CompareUOM(name, "calories/cubic millimetre") == 0) {
      myReturn = 298;
    } else if (CompareUOM(name, "calories/gram") == 0) {
      myReturn = 289;
    } else if (CompareUOM(name, "calories/gram degree kelvin") == 0) {
      myReturn = 290;
    } else if (CompareUOM(name, "calories/gram mol degree celsius") == 0) {
      myReturn = 299;
    } else if (CompareUOM(name, "calories/hour centimetre degree celsius") == 0) {
      myReturn = 291;
    } else if (CompareUOM(name, "calories/hour centimetre squared") == 0) {
      myReturn = 292;
    } else if (CompareUOM(name, "calories/hour cubic centimetre") == 0) {
      myReturn = 294;
    } else if (CompareUOM(name, "calories/hour square centimetre deg c") == 0) {
      myReturn = 293;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "calories/kilogram") == 0) {
      myReturn = 295;
    } else if (CompareUOM(name, "calories/milliliter") == 0) {
      myReturn = 297;
    } else if (CompareUOM(name, "calories/pound mass") == 0) {
      myReturn = 296;
    } else if (CompareUOM(name, "calories/second centimetre deg c") == 0) {
      myReturn = 300;
    } else if (CompareUOM(name, "calories/second cubic centimetre") == 0) {
      myReturn = 302;
    } else if (CompareUOM(name, "calories/second square centimetre deg c") == 0) {
      myReturn = 301;
    } else if (CompareUOM(name, "candela") == 0) {
      myReturn = 27;
    } else if (CompareUOM(name, "candelas/square metre") == 0) {
      myReturn = 28;
    } else if (CompareUOM(name, "capture unit") == 0) {
      myReturn = 336;
    } else if (CompareUOM(name, "carat") == 0) {
      myReturn = 335;
    } else if (CompareUOM(name, "cd") == 0) {
      myReturn = 27;
    } else if (CompareUOM(name, "cd/m2") == 0) {
      myReturn = 28;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "centesimal minute") == 0) {
      myReturn = 305;
    } else if (CompareUOM(name, "centesimal second") == 0) {
      myReturn = 303;
    } else if (CompareUOM(name, "centieuclid") == 0) {
      myReturn = 304;
    } else if (CompareUOM(name, "centimeter per year") == 0) {
      myReturn = 316;
    } else if (CompareUOM(name, "centimetre") == 0) {
      myReturn = 315;
    } else if (CompareUOM(name, "centimetre/second") == 0) {
      myReturn = 317;
    } else if (CompareUOM(name, "centimetre/second squared") == 0) {
      myReturn = 318;
    } else if (CompareUOM(name, "centimetres fourth") == 0) {
      myReturn = 330;
    } else if (CompareUOM(name, "centimetres squared/gram") == 0) {
      myReturn = 320;
    } else if (CompareUOM(name, "centimetres squared/second") == 0) {
      myReturn = 321;
    } else if (CompareUOM(name, "centipoise") == 0) {
      myReturn = 332;
    } else if (CompareUOM(name, "centistoke") == 0) {
      myReturn = 334;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "ceuc") == 0) {
      myReturn = 1218;
    } else if (CompareUOM(name, "ch") == 0) {
      myReturn = 306;
    } else if (CompareUOM(name, "ch hours") == 0) {
      myReturn = 307;
    } else if (CompareUOM(name, "change in degrees celsius") == 0) {
      myReturn = 361;
    } else if (CompareUOM(name, "change in degrees fahrenheit") == 0) {
      myReturn = 362;
    } else if (CompareUOM(name, "change in degrees kelvin") == 0) {
      myReturn = 363;
    } else if (CompareUOM(name, "change in degrees rankine") == 0) {
      myReturn = 364;
    } else if (CompareUOM(name, "cheval vapeur") == 0) {
      myReturn = 341;
    } else if (CompareUOM(name, "chus") == 0) {
      myReturn = 312;
    } else if (CompareUOM(name, "ci") == 0) {
      myReturn = 314;
    } else if (CompareUOM(name, "clarke chain") == 0) {
      myReturn = 310;
    } else if (CompareUOM(name, "clarke link") == 0) {
      myReturn = 787;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "clarkes yard") == 0) {
      myReturn = 1223;
    } else if (CompareUOM(name, "cm") == 0) {
      myReturn = 315;
    } else if (CompareUOM(name, "cm of water at 4 degc.") == 0) {
      myReturn = 331;
    } else if (CompareUOM(name, "cm/a") == 0) {
      myReturn = 316;
    } else if (CompareUOM(name, "cm/s") == 0) {
      myReturn = 317;
    } else if (CompareUOM(name, "cm/s2") == 0) {
      myReturn = 318;
    } else if (CompareUOM(name, "cm2") == 0) {
      myReturn = 319;
    } else if (CompareUOM(name, "cm2/g") == 0) {
      myReturn = 320;
    } else if (CompareUOM(name, "cm2/s") == 0) {
      myReturn = 321;
    } else if (CompareUOM(name, "cm3") == 0) {
      myReturn = 322;
    } else if (CompareUOM(name, "cm3/cm3") == 0) {
      myReturn = 324;
    } else if (CompareUOM(name, "cm3/g") == 0) {
      myReturn = 325;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "cm3/h") == 0) {
      myReturn = 326;
    } else if (CompareUOM(name, "cm3/m3") == 0) {
      myReturn = 327;
    } else if (CompareUOM(name, "cm3/min") == 0) {
      myReturn = 328;
    } else if (CompareUOM(name, "cm3/s") == 0) {
      myReturn = 329;
    } else if (CompareUOM(name, "cm4") == 0) {
      myReturn = 330;
    } else if (CompareUOM(name, "coulomb") == 0) {
      myReturn = 22;
    } else if (CompareUOM(name, "coulomb metres") == 0) {
      myReturn = 23;
    } else if (CompareUOM(name, "coulomb per gram") == 0) {
      myReturn = 283;
    } else if (CompareUOM(name, "coulomb per kilogram") == 0) {
      myReturn = 24;
    } else if (CompareUOM(name, "coulombs/cubic centimeter") == 0) {
      myReturn = 282;
    } else if (CompareUOM(name, "coulombs/cubic metre") == 0) {
      myReturn = 26;
    } else if (CompareUOM(name, "coulombs/cubic millimetre") == 0) {
      myReturn = 285;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "coulombs/square centimetre") == 0) {
      myReturn = 281;
    } else if (CompareUOM(name, "coulombs/square metre") == 0) {
      myReturn = 25;
    } else if (CompareUOM(name, "coulombs/square millimetre") == 0) {
      myReturn = 284;
    } else if (CompareUOM(name, "cp") == 0) {
      myReturn = 332;
    } else if (CompareUOM(name, "cu") == 0) {
      myReturn = 336;
    } else if (CompareUOM(name, "cu in") == 0) {
      myReturn = 601;
    } else if (CompareUOM(name, "cubic centimeter per hour") == 0) {
      myReturn = 326;
    } else if (CompareUOM(name, "cubic centimeter per minute") == 0) {
      myReturn = 328;
    } else if (CompareUOM(name, "cubic centimeter per second") == 0) {
      myReturn = 329;
    } else if (CompareUOM(name, "cubic centimeter per thirty minutes") == 0) {
      myReturn = 323;
    } else if (CompareUOM(name, "cubic centimeters/ cubic centimetres") == 0) {
      myReturn = 324;
    } else if (CompareUOM(name, "cubic centimetre") == 0) {
      myReturn = 322;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "cubic centimetre/cubic metre") == 0) {
      myReturn = 327;
    } else if (CompareUOM(name, "cubic centimetres/gram") == 0) {
      myReturn = 325;
    } else if (CompareUOM(name, "cubic decimetre") == 0) {
      myReturn = 394;
    } else if (CompareUOM(name, "cubic decimetres/100 kilometres") == 0) {
      myReturn = 395;
    } else if (CompareUOM(name, "cubic decimetres/cubic metre") == 0) {
      myReturn = 399;
    } else if (CompareUOM(name, "cubic decimetres/kilogram") == 0) {
      myReturn = 396;
    } else if (CompareUOM(name, "cubic decimetres/kilogram mole") == 0) {
      myReturn = 401;
    } else if (CompareUOM(name, "cubic decimetres/kilowatt hour") == 0) {
      myReturn = 397;
    } else if (CompareUOM(name, "cubic decimetres/megajoule") == 0) {
      myReturn = 400;
    } else if (CompareUOM(name, "cubic decimetres/metre") == 0) {
      myReturn = 398;
    } else if (CompareUOM(name, "cubic decimetres/second") == 0) {
      myReturn = 402;
    } else if (CompareUOM(name, "cubic decimetres/second/second") == 0) {
      myReturn = 403;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "cubic decimetres/ton") == 0) {
      myReturn = 404;
    } else if (CompareUOM(name, "cubic feet") == 0) {
      myReturn = 337;
    } else if (CompareUOM(name, "cubic feet at standard conditions") == 0) {
      myReturn = 461;
    } else if (CompareUOM(name, "cubic feet per 94 pound sack") == 0) {
      myReturn = 479;
    } else if (CompareUOM(name, "cubic feet per kilogram") == 0) {
      myReturn = 470;
    } else if (CompareUOM(name, "cubic feet/barrel") == 0) {
      myReturn = 462;
    } else if (CompareUOM(name, "cubic feet/cubic foot") == 0) {
      myReturn = 467;
    } else if (CompareUOM(name, "cubic feet/day") == 0) {
      myReturn = 463;
    } else if (CompareUOM(name, "cubic feet/day foot psi") == 0) {
      myReturn = 464;
    } else if (CompareUOM(name, "cubic feet/day/day") == 0) {
      myReturn = 465;
    } else if (CompareUOM(name, "cubic feet/foot") == 0) {
      myReturn = 466;
    } else if (CompareUOM(name, "cubic feet/hour") == 0) {
      myReturn = 468;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "cubic feet/hour/hour") == 0) {
      myReturn = 469;
    } else if (CompareUOM(name, "cubic feet/min square foot") == 0) {
      myReturn = 473;
    } else if (CompareUOM(name, "cubic feet/minute") == 0) {
      myReturn = 472;
    } else if (CompareUOM(name, "cubic feet/minute/minute") == 0) {
      myReturn = 474;
    } else if (CompareUOM(name, "cubic feet/mole (pound mass)") == 0) {
      myReturn = 475;
    } else if (CompareUOM(name, "cubic feet/pound mass") == 0) {
      myReturn = 471;
    } else if (CompareUOM(name, "cubic feet/second") == 0) {
      myReturn = 476;
    } else if (CompareUOM(name, "cubic feet/second square foot") == 0) {
      myReturn = 477;
    } else if (CompareUOM(name, "cubic feet/second/second") == 0) {
      myReturn = 478;
    } else if (CompareUOM(name, "cubic feet/std cubic foot, 60 deg f") == 0) {
      myReturn = 480;
    } else if (CompareUOM(name, "cubic inch") == 0) {
      myReturn = 338;
    } else if (CompareUOM(name, "cubic inches") == 0) {
      myReturn = 601;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "cubic inches/foot") == 0) {
      myReturn = 602;
    } else if (CompareUOM(name, "cubic kilometres") == 0) {
      myReturn = 682;
    } else if (CompareUOM(name, "cubic meter per day per (pound per square inch)") == 0) {
      myReturn = 836;
    } else if (CompareUOM(name, "cubic meter per day per bar") == 0) {
      myReturn = 814;
    } else if (CompareUOM(name, "cubic meter per day per kilopascal") == 0) {
      myReturn = 828;
    } else if (CompareUOM(name, "cubic meter per day per meter") == 0) {
      myReturn = 821;
    } else if (CompareUOM(name, "cubic meter per hour per bar") == 0) {
      myReturn = 815;
    } else if (CompareUOM(name, "cubic meter per hour per meter") == 0) {
      myReturn = 825;
    } else if (CompareUOM(name, "cubic meter per minute") == 0) {
      myReturn = 834;
    } else if (CompareUOM(name, "cubic meter per minute per bar") == 0) {
      myReturn = 816;
    } else if (CompareUOM(name, "cubic meter per second per foot") == 0) {
      myReturn = 837;
    } else if (CompareUOM(name, "cubic meter per square meter") == 0) {
      myReturn = 832;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "cubic meters per uk ton") == 0) {
      myReturn = 841;
    } else if (CompareUOM(name, "cubic meters per us ton") == 0) {
      myReturn = 842;
    } else if (CompareUOM(name, "cubic metres") == 0) {
      myReturn = 78;
    } else if (CompareUOM(name, "cubic metres at std condition (0 deg c)") == 0) {
      myReturn = 812;
    } else if (CompareUOM(name, "cubic metres at std condition (15 deg c)") == 0) {
      myReturn = 813;
    } else if (CompareUOM(name, "cubic metres/centipoise day kilopascal") == 0) {
      myReturn = 817;
    } else if (CompareUOM(name, "cubic metres/centipoise pascal second") == 0) {
      myReturn = 818;
    } else if (CompareUOM(name, "cubic metres/cubic metre") == 0) {
      myReturn = 833;
    } else if (CompareUOM(name, "cubic metres/day") == 0) {
      myReturn = 819;
    } else if (CompareUOM(name, "cubic metres/day kilopascal") == 0) {
      myReturn = 820;
    } else if (CompareUOM(name, "cubic metres/day/day") == 0) {
      myReturn = 822;
    } else if (CompareUOM(name, "cubic metres/gram") == 0) {
      myReturn = 823;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "cubic metres/hectare metre") == 0) {
      myReturn = 826;
    } else if (CompareUOM(name, "cubic metres/hour") == 0) {
      myReturn = 824;
    } else if (CompareUOM(name, "cubic metres/joule") == 0) {
      myReturn = 79;
    } else if (CompareUOM(name, "cubic metres/kilogram") == 0) {
      myReturn = 80;
    } else if (CompareUOM(name, "cubic metres/kilometre") == 0) {
      myReturn = 827;
    } else if (CompareUOM(name, "cubic metres/kilowatt hour") == 0) {
      myReturn = 830;
    } else if (CompareUOM(name, "cubic metres/metre") == 0) {
      myReturn = 831;
    } else if (CompareUOM(name, "cubic metres/mole") == 0) {
      myReturn = 81;
    } else if (CompareUOM(name, "cubic metres/mole (kilogram)") == 0) {
      myReturn = 835;
    } else if (CompareUOM(name, "cubic metres/pascal second squared") == 0) {
      myReturn = 84;
    } else if (CompareUOM(name, "cubic metres/second") == 0) {
      myReturn = 85;
    } else if (CompareUOM(name, "cubic metres/second metre") == 0) {
      myReturn = 838;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "cubic metres/second pascal") == 0) {
      myReturn = 82;
    } else if (CompareUOM(name, "cubic metres/second square metre") == 0) {
      myReturn = 839;
    } else if (CompareUOM(name, "cubic metres/seconds squared") == 0) {
      myReturn = 86;
    } else if (CompareUOM(name, "cubic metres/std cubic metres, 0 deg c") == 0) {
      myReturn = 87;
    } else if (CompareUOM(name, "cubic metres/std cubic metres, 15 deg c") == 0) {
      myReturn = 88;
    } else if (CompareUOM(name, "cubic metres/tonne") == 0) {
      myReturn = 840;
    } else if (CompareUOM(name, "cubic mile") == 0) {
      myReturn = 340;
    } else if (CompareUOM(name, "cubic millimetres") == 0) {
      myReturn = 933;
    } else if (CompareUOM(name, "cubic millimetres/joule") == 0) {
      myReturn = 934;
    } else if (CompareUOM(name, "cubic yard") == 0) {
      myReturn = 339;
    } else if (CompareUOM(name, "curie") == 0) {
      myReturn = 314;
    } else if (CompareUOM(name, "cv hours") == 0) {
      myReturn = 342;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "cycle") == 0) {
      myReturn = 280;
    } else if (CompareUOM(name, "cycles/second") == 0) {
      myReturn = 286;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_d(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "d") == 0) {
    myReturn = 346;
  } else if (CompareUOM(name, "d.ft") == 0) {
    myReturn = 347;
  } else if (CompareUOM(name, "d.m") == 0) {
    myReturn = 348;
  } else if (CompareUOM(name, "d/bbl") == 0) {
    myReturn = 349;
  } else if (CompareUOM(name, "d/ft3") == 0) {
    myReturn = 350;
  } else if (CompareUOM(name, "d/m3") == 0) {
    myReturn = 352;
  } else if (CompareUOM(name, "dapi") == 0) {
    myReturn = 355;
  } else if (CompareUOM(name, "darcy") == 0) {
    myReturn = 345;
  } else if (CompareUOM(name, "darcy foot") == 0) {
    myReturn = 347;
  } else if (CompareUOM(name, "darcy metre") == 0) {
    myReturn = 348;
  } else if (CompareUOM(name, "day") == 0) {
    myReturn = 346;
  } else if (CompareUOM(name, "day per barrel") == 0) {
    myReturn = 349;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "day per thousand cubic feet") == 0) {
      myReturn = 351;
    } else if (CompareUOM(name, "days/cubic foot") == 0) {
      myReturn = 350;
    } else if (CompareUOM(name, "days/cubic metre") == 0) {
      myReturn = 352;
    } else if (CompareUOM(name, "db") == 0) {
      myReturn = 356;
    } else if (CompareUOM(name, "db/ft") == 0) {
      myReturn = 357;
    } else if (CompareUOM(name, "db/km") == 0) {
      myReturn = 359;
    } else if (CompareUOM(name, "db/m") == 0) {
      myReturn = 358;
    } else if (CompareUOM(name, "decanewton metres") == 0) {
      myReturn = 354;
    } else if (CompareUOM(name, "decanewtons") == 0) {
      myReturn = 353;
    } else if (CompareUOM(name, "decibel") == 0) {
      myReturn = 356;
    } else if (CompareUOM(name, "decibels/foot") == 0) {
      myReturn = 357;
    } else if (CompareUOM(name, "decibels/kilometre") == 0) {
      myReturn = 359;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "decibels/metre") == 0) {
      myReturn = 358;
    } else if (CompareUOM(name, "decibels/octave") == 0) {
      myReturn = 360;
    } else if (CompareUOM(name, "decimeter per second") == 0) {
      myReturn = 393;
    } else if (CompareUOM(name, "decimetre") == 0) {
      myReturn = 392;
    } else if (CompareUOM(name, "decinewton metres") == 0) {
      myReturn = 405;
    } else if (CompareUOM(name, "dega") == 0) {
      myReturn = 365;
    } else if (CompareUOM(name, "dega/ft") == 0) {
      myReturn = 369;
    } else if (CompareUOM(name, "dega/ft(100)") == 0) {
      myReturn = 366;
    } else if (CompareUOM(name, "dega/h") == 0) {
      myReturn = 370;
    } else if (CompareUOM(name, "dega/m") == 0) {
      myReturn = 371;
    } else if (CompareUOM(name, "dega/min") == 0) {
      myReturn = 372;
    } else if (CompareUOM(name, "dega/s") == 0) {
      myReturn = 373;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "degc") == 0) {
      myReturn = 374;
    } else if (CompareUOM(name, "degc.m2.h/kcal") == 0) {
      myReturn = 375;
    } else if (CompareUOM(name, "degc/ft") == 0) {
      myReturn = 377;
    } else if (CompareUOM(name, "degc/h") == 0) {
      myReturn = 378;
    } else if (CompareUOM(name, "degc/km") == 0) {
      myReturn = 379;
    } else if (CompareUOM(name, "degc/m") == 0) {
      myReturn = 380;
    } else if (CompareUOM(name, "degc/min") == 0) {
      myReturn = 381;
    } else if (CompareUOM(name, "degc/s") == 0) {
      myReturn = 382;
    } else if (CompareUOM(name, "degf") == 0) {
      myReturn = 383;
    } else if (CompareUOM(name, "degf.ft2.h/btu") == 0) {
      myReturn = 384;
    } else if (CompareUOM(name, "degf/ft") == 0) {
      myReturn = 386;
    } else if (CompareUOM(name, "degf/ft(100)") == 0) {
      myReturn = 385;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "degf/h") == 0) {
      myReturn = 387;
    } else if (CompareUOM(name, "degf/m") == 0) {
      myReturn = 388;
    } else if (CompareUOM(name, "degf/min") == 0) {
      myReturn = 389;
    } else if (CompareUOM(name, "degf/s") == 0) {
      myReturn = 390;
    } else if (CompareUOM(name, "degr") == 0) {
      myReturn = 391;
    } else if (CompareUOM(name, "degree fahrenheit") == 0) {
      myReturn = 383;
    } else if (CompareUOM(name, "degree of an angle") == 0) {
      myReturn = 365;
    } else if (CompareUOM(name, "degrees c square metres hours/kilocal") == 0) {
      myReturn = 375;
    } else if (CompareUOM(name, "degrees celsius") == 0) {
      myReturn = 374;
    } else if (CompareUOM(name, "degrees celsius per foot") == 0) {
      myReturn = 377;
    } else if (CompareUOM(name, "degrees celsius per hour") == 0) {
      myReturn = 378;
    } else if (CompareUOM(name, "degrees celsius per hundred metre") == 0) {
      myReturn = 376;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "degrees celsius per minute") == 0) {
      myReturn = 381;
    } else if (CompareUOM(name, "degrees celsius per second") == 0) {
      myReturn = 382;
    } else if (CompareUOM(name, "degrees celsius/kilometre") == 0) {
      myReturn = 379;
    } else if (CompareUOM(name, "degrees celsius/metre") == 0) {
      myReturn = 380;
    } else if (CompareUOM(name, "degrees f square feet hours/btu") == 0) {
      myReturn = 384;
    } else if (CompareUOM(name, "degrees fahrenheit per hour") == 0) {
      myReturn = 387;
    } else if (CompareUOM(name, "degrees fahrenheit per meter") == 0) {
      myReturn = 388;
    } else if (CompareUOM(name, "degrees fahrenheit per minute") == 0) {
      myReturn = 389;
    } else if (CompareUOM(name, "degrees fahrenheit per second") == 0) {
      myReturn = 390;
    } else if (CompareUOM(name, "degrees fahrenheit/100 feet.") == 0) {
      myReturn = 385;
    } else if (CompareUOM(name, "degrees fahrenheit/foot") == 0) {
      myReturn = 386;
    } else if (CompareUOM(name, "degrees kelvin square metres/kilowatt") == 0) {
      myReturn = 616;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "degrees kelvin/metre") == 0) {
      myReturn = 50;
    } else if (CompareUOM(name, "degrees of an angle per hour") == 0) {
      myReturn = 370;
    } else if (CompareUOM(name, "degrees of an angle per second") == 0) {
      myReturn = 373;
    } else if (CompareUOM(name, "degrees of an angle per thirty feet") == 0) {
      myReturn = 367;
    } else if (CompareUOM(name, "degrees of an angle/100 feet") == 0) {
      myReturn = 366;
    } else if (CompareUOM(name, "degrees of an angle/30 metres") == 0) {
      myReturn = 368;
    } else if (CompareUOM(name, "degrees of an angle/foot") == 0) {
      myReturn = 369;
    } else if (CompareUOM(name, "degrees of an angle/metre") == 0) {
      myReturn = 371;
    } else if (CompareUOM(name, "degrees of an angle/minute") == 0) {
      myReturn = 372;
    } else if (CompareUOM(name, "degrees rankine") == 0) {
      myReturn = 391;
    } else if (CompareUOM(name, "delta kelvin per watt") == 0) {
      myReturn = 52;
    } else if (CompareUOM(name, "dm") == 0) {
      myReturn = 392;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "dm/s") == 0) {
      myReturn = 393;
    } else if (CompareUOM(name, "dm3") == 0) {
      myReturn = 720;
    } else if (CompareUOM(name, "dm3/kg") == 0) {
      myReturn = 396;
    } else if (CompareUOM(name, "dm3/km(100)") == 0) {
      myReturn = 395;
    } else if (CompareUOM(name, "dm3/kw.h") == 0) {
      myReturn = 397;
    } else if (CompareUOM(name, "dm3/m") == 0) {
      myReturn = 398;
    } else if (CompareUOM(name, "dm3/m3") == 0) {
      myReturn = 399;
    } else if (CompareUOM(name, "dm3/mj") == 0) {
      myReturn = 400;
    } else if (CompareUOM(name, "dm3/s") == 0) {
      myReturn = 402;
    } else if (CompareUOM(name, "dm3/s2") == 0) {
      myReturn = 403;
    } else if (CompareUOM(name, "dm3/t") == 0) {
      myReturn = 404;
    } else if (CompareUOM(name, "dn.m") == 0) {
      myReturn = 405;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "dyne centimetre squared") == 0) {
      myReturn = 407;
    } else if (CompareUOM(name, "dyne seconds/square centimetre") == 0) {
      myReturn = 408;
    } else if (CompareUOM(name, "dynes") == 0) {
      myReturn = 406;
    } else if (CompareUOM(name, "dynes/centimetre") == 0) {
      myReturn = 409;
    } else if (CompareUOM(name, "dynes/centimetre fourth/gram cm cubed") == 0) {
      myReturn = 411;
    } else if (CompareUOM(name, "dynes/square centimetre") == 0) {
      myReturn = 410;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_e(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "ehp") == 0) {
    myReturn = 413;
  } else if (CompareUOM(name, "ej") == 0) {
    myReturn = 414;
  } else if (CompareUOM(name, "ej/a") == 0) {
    myReturn = 415;
  } else if (CompareUOM(name, "electric horsepower") == 0) {
    myReturn = 413;
  } else if (CompareUOM(name, "electron volts") == 0) {
    myReturn = 424;
  } else if (CompareUOM(name, "eq") == 0) {
    myReturn = 29;
  } else if (CompareUOM(name, "eq/kg") == 0) {
    myReturn = 30;
  } else if (CompareUOM(name, "eq/l") == 0) {
    myReturn = 416;
  } else if (CompareUOM(name, "eq/m3") == 0) {
    myReturn = 31;
  } else if (CompareUOM(name, "equivalent") == 0) {
    myReturn = 29;
  } else if (CompareUOM(name, "equivalent per kilogram") == 0) {
    myReturn = 30;
  } else if (CompareUOM(name, "equivalents per cubic metre") == 0) {
    myReturn = 31;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "equivalents/ liter") == 0) {
      myReturn = 416;
    } else if (CompareUOM(name, "ergs") == 0) {
      myReturn = 417;
    } else if (CompareUOM(name, "ergs/cubic centimetre") == 0) {
      myReturn = 420;
    } else if (CompareUOM(name, "ergs/cubic metre") == 0) {
      myReturn = 423;
    } else if (CompareUOM(name, "ergs/gram") == 0) {
      myReturn = 421;
    } else if (CompareUOM(name, "ergs/kilogram") == 0) {
      myReturn = 422;
    } else if (CompareUOM(name, "ergs/square centimetre") == 0) {
      myReturn = 419;
    } else if (CompareUOM(name, "ergs/year") == 0) {
      myReturn = 418;
    } else if (CompareUOM(name, "euc") == 0) {
      myReturn = 1191;
    } else if (CompareUOM(name, "euclid") == 0) {
      myReturn = 32;
    } else if (CompareUOM(name, "ev") == 0) {
      myReturn = 424;
    } else if (CompareUOM(name, "exajoule") == 0) {
      myReturn = 414;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "exajoules/year") == 0) {
      myReturn = 415;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_f(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "f") == 0) {
    myReturn = 33;
  } else if (CompareUOM(name, "f/m") == 0) {
    myReturn = 34;
  } else if (CompareUOM(name, "farad") == 0) {
    myReturn = 33;
  } else if (CompareUOM(name, "farads/metre") == 0) {
    myReturn = 34;
  } else if (CompareUOM(name, "fathoms") == 0) {
    myReturn = 425;
  } else if (CompareUOM(name, "fc") == 0) {
    myReturn = 426;
  } else if (CompareUOM(name, "feet") == 0) {
    myReturn = 433;
  } else if (CompareUOM(name, "feet per 100 feet") == 0) {
    myReturn = 441;
  } else if (CompareUOM(name, "feet per feet") == 0) {
    myReturn = 445;
  } else if (CompareUOM(name, "feet/barrel") == 0) {
    myReturn = 442;
  } else if (CompareUOM(name, "feet/cubic foot") == 0) {
    myReturn = 446;
  } else if (CompareUOM(name, "feet/day") == 0) {
    myReturn = 443;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "feet/degree fahrenheit") == 0) {
      myReturn = 444;
    } else if (CompareUOM(name, "feet/hour") == 0) {
      myReturn = 448;
    } else if (CompareUOM(name, "feet/inch") == 0) {
      myReturn = 449;
    } else if (CompareUOM(name, "feet/metre") == 0) {
      myReturn = 450;
    } else if (CompareUOM(name, "feet/mile") == 0) {
      myReturn = 451;
    } else if (CompareUOM(name, "feet/minute") == 0) {
      myReturn = 452;
    } else if (CompareUOM(name, "feet/second") == 0) {
      myReturn = 454;
    } else if (CompareUOM(name, "feet/second squared") == 0) {
      myReturn = 455;
    } else if (CompareUOM(name, "feet/us gallon") == 0) {
      myReturn = 447;
    } else if (CompareUOM(name, "femtocoulomb") == 0) {
      myReturn = 426;
    } else if (CompareUOM(name, "femtometer") == 0) {
      myReturn = 430;
    } else if (CompareUOM(name, "flops") == 0) {
      myReturn = 429;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "flozuk") == 0) {
      myReturn = 427;
    } else if (CompareUOM(name, "flozus") == 0) {
      myReturn = 428;
    } else if (CompareUOM(name, "fm") == 0) {
      myReturn = 430;
    } else if (CompareUOM(name, "foot") == 0) {
      myReturn = 433;
    } else if (CompareUOM(name, "foot per microsecond") == 0) {
      myReturn = 456;
    } else if (CompareUOM(name, "foot per millisecond") == 0) {
      myReturn = 453;
    } else if (CompareUOM(name, "foot poundal") == 0) {
      myReturn = 1048;
    } else if (CompareUOM(name, "foot pounds force") == 0) {
      myReturn = 434;
    } else if (CompareUOM(name, "foot pounds force/barrel") == 0) {
      myReturn = 435;
    } else if (CompareUOM(name, "foot pounds force/minute") == 0) {
      myReturn = 438;
    } else if (CompareUOM(name, "foot pounds force/pound mass") == 0) {
      myReturn = 437;
    } else if (CompareUOM(name, "foot pounds force/second") == 0) {
      myReturn = 439;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "foot pounds force/square inch") == 0) {
      myReturn = 738;
    } else if (CompareUOM(name, "foot pounds force/us gallon") == 0) {
      myReturn = 436;
    } else if (CompareUOM(name, "foot pounds mass/second") == 0) {
      myReturn = 755;
    } else if (CompareUOM(name, "foot-pound mass") == 0) {
      myReturn = 440;
    } else if (CompareUOM(name, "footcandle seconds") == 0) {
      myReturn = 432;
    } else if (CompareUOM(name, "footcandles") == 0) {
      myReturn = 431;
    } else if (CompareUOM(name, "ft") == 0) {
      myReturn = 433;
    } else if (CompareUOM(name, "ft.lbf") == 0) {
      myReturn = 434;
    } else if (CompareUOM(name, "ft.lbf/bbl") == 0) {
      myReturn = 435;
    } else if (CompareUOM(name, "ft.lbf/galus") == 0) {
      myReturn = 436;
    } else if (CompareUOM(name, "ft.lbf/lbm") == 0) {
      myReturn = 437;
    } else if (CompareUOM(name, "ft.lbf/min") == 0) {
      myReturn = 438;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "ft.lbf/s") == 0) {
      myReturn = 439;
    } else if (CompareUOM(name, "ft.lbm") == 0) {
      myReturn = 440;
    } else if (CompareUOM(name, "ft/bbl") == 0) {
      myReturn = 442;
    } else if (CompareUOM(name, "ft/d") == 0) {
      myReturn = 443;
    } else if (CompareUOM(name, "ft/degf") == 0) {
      myReturn = 444;
    } else if (CompareUOM(name, "ft/ft") == 0) {
      myReturn = 445;
    } else if (CompareUOM(name, "ft/ft3") == 0) {
      myReturn = 446;
    } else if (CompareUOM(name, "ft/galus") == 0) {
      myReturn = 447;
    } else if (CompareUOM(name, "ft/h") == 0) {
      myReturn = 448;
    } else if (CompareUOM(name, "ft/in") == 0) {
      myReturn = 449;
    } else if (CompareUOM(name, "ft/m") == 0) {
      myReturn = 450;
    } else if (CompareUOM(name, "ft/mi") == 0) {
      myReturn = 451;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "ft/min") == 0) {
      myReturn = 452;
    } else if (CompareUOM(name, "ft/ms") == 0) {
      myReturn = 453;
    } else if (CompareUOM(name, "ft/s") == 0) {
      myReturn = 454;
    } else if (CompareUOM(name, "ft/s2") == 0) {
      myReturn = 455;
    } else if (CompareUOM(name, "ft/us") == 0) {
      myReturn = 456;
    } else if (CompareUOM(name, "ft2") == 0) {
      myReturn = 1112;
    } else if (CompareUOM(name, "ft2/h") == 0) {
      myReturn = 458;
    } else if (CompareUOM(name, "ft2/in3") == 0) {
      myReturn = 459;
    } else if (CompareUOM(name, "ft2/s") == 0) {
      myReturn = 460;
    } else if (CompareUOM(name, "ft3") == 0) {
      myReturn = 337;
    } else if (CompareUOM(name, "ft3(std,60f)") == 0) {
      myReturn = 337;
    } else if (CompareUOM(name, "ft3/bbl") == 0) {
      myReturn = 462;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "ft3/d") == 0) {
      myReturn = 463;
    } else if (CompareUOM(name, "ft3/d.ft.psi") == 0) {
      myReturn = 464;
    } else if (CompareUOM(name, "ft3/d2") == 0) {
      myReturn = 465;
    } else if (CompareUOM(name, "ft3/ft") == 0) {
      myReturn = 466;
    } else if (CompareUOM(name, "ft3/ft3") == 0) {
      myReturn = 467;
    } else if (CompareUOM(name, "ft3/h") == 0) {
      myReturn = 468;
    } else if (CompareUOM(name, "ft3/h2") == 0) {
      myReturn = 469;
    } else if (CompareUOM(name, "ft3/kg") == 0) {
      myReturn = 470;
    } else if (CompareUOM(name, "ft3/lbm") == 0) {
      myReturn = 471;
    } else if (CompareUOM(name, "ft3/min") == 0) {
      myReturn = 472;
    } else if (CompareUOM(name, "ft3/min.ft2") == 0) {
      myReturn = 473;
    } else if (CompareUOM(name, "ft3/min2") == 0) {
      myReturn = 474;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "ft3/s") == 0) {
      myReturn = 476;
    } else if (CompareUOM(name, "ft3/s.ft2") == 0) {
      myReturn = 477;
    } else if (CompareUOM(name, "ft3/s2") == 0) {
      myReturn = 478;
    } else if (CompareUOM(name, "ftam") == 0) {
      myReturn = 490;
    } else if (CompareUOM(name, "ftcla") == 0) {
      myReturn = 484;
    } else if (CompareUOM(name, "ftus") == 0) {
      myReturn = 492;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_g(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "g") == 0) {
    myReturn = 493;
  } else if (CompareUOM(name, "g.ft/cm3.s") == 0) {
    myReturn = 494;
  } else if (CompareUOM(name, "g/cm3") == 0) {
    myReturn = 495;
  } else if (CompareUOM(name, "g/cm4") == 0) {
    myReturn = 496;
  } else if (CompareUOM(name, "g/dm3") == 0) {
    myReturn = 501;
  } else if (CompareUOM(name, "g/galuk") == 0) {
    myReturn = 498;
  } else if (CompareUOM(name, "g/galus") == 0) {
    myReturn = 499;
  } else if (CompareUOM(name, "g/kg") == 0) {
    myReturn = 500;
  } else if (CompareUOM(name, "g/l") == 0) {
    myReturn = 501;
  } else if (CompareUOM(name, "g/m3") == 0) {
    myReturn = 502;
  } else if (CompareUOM(name, "g/s") == 0) {
    myReturn = 503;
  } else if (CompareUOM(name, "galileo") == 0) {
    myReturn = 505;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "galuk") == 0) {
      myReturn = 507;
    } else if (CompareUOM(name, "galuk/d") == 0) {
      myReturn = 508;
    } else if (CompareUOM(name, "galuk/ft3") == 0) {
      myReturn = 509;
    } else if (CompareUOM(name, "galuk/h") == 0) {
      myReturn = 510;
    } else if (CompareUOM(name, "galuk/h.ft") == 0) {
      myReturn = 511;
    } else if (CompareUOM(name, "galuk/h.ft2") == 0) {
      myReturn = 512;
    } else if (CompareUOM(name, "galuk/h.in") == 0) {
      myReturn = 513;
    } else if (CompareUOM(name, "galuk/h.in2") == 0) {
      myReturn = 514;
    } else if (CompareUOM(name, "galuk/h2") == 0) {
      myReturn = 515;
    } else if (CompareUOM(name, "galuk/lbm") == 0) {
      myReturn = 517;
    } else if (CompareUOM(name, "galuk/mi") == 0) {
      myReturn = 519;
    } else if (CompareUOM(name, "galuk/min") == 0) {
      myReturn = 520;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "galuk/min.ft") == 0) {
      myReturn = 521;
    } else if (CompareUOM(name, "galuk/min.ft2") == 0) {
      myReturn = 522;
    } else if (CompareUOM(name, "galuk/min2") == 0) {
      myReturn = 523;
    } else if (CompareUOM(name, "galus") == 0) {
      myReturn = 524;
    } else if (CompareUOM(name, "galus/bbl") == 0) {
      myReturn = 526;
    } else if (CompareUOM(name, "galus/d") == 0) {
      myReturn = 527;
    } else if (CompareUOM(name, "galus/ft") == 0) {
      myReturn = 528;
    } else if (CompareUOM(name, "galus/ft3") == 0) {
      myReturn = 529;
    } else if (CompareUOM(name, "galus/h") == 0) {
      myReturn = 530;
    } else if (CompareUOM(name, "galus/h.ft") == 0) {
      myReturn = 531;
    } else if (CompareUOM(name, "galus/h.ft2") == 0) {
      myReturn = 532;
    } else if (CompareUOM(name, "galus/h.in") == 0) {
      myReturn = 533;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "galus/h.in2") == 0) {
      myReturn = 534;
    } else if (CompareUOM(name, "galus/h2") == 0) {
      myReturn = 535;
    } else if (CompareUOM(name, "galus/lbm") == 0) {
      myReturn = 537;
    } else if (CompareUOM(name, "galus/mi") == 0) {
      myReturn = 539;
    } else if (CompareUOM(name, "galus/min") == 0) {
      myReturn = 540;
    } else if (CompareUOM(name, "galus/min.ft") == 0) {
      myReturn = 541;
    } else if (CompareUOM(name, "galus/min.ft2") == 0) {
      myReturn = 542;
    } else if (CompareUOM(name, "galus/min2") == 0) {
      myReturn = 543;
    } else if (CompareUOM(name, "galus/tonuk") == 0) {
      myReturn = 545;
    } else if (CompareUOM(name, "galus/tonus") == 0) {
      myReturn = 546;
    } else if (CompareUOM(name, "gamma") == 0) {
      myReturn = 547;
    } else if (CompareUOM(name, "gapi") == 0) {
      myReturn = 36;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "gas unit") == 0) {
      myReturn = 35;
    } else if (CompareUOM(name, "gauss") == 0) {
      myReturn = 548;
    } else if (CompareUOM(name, "gbq") == 0) {
      myReturn = 549;
    } else if (CompareUOM(name, "german legal metre") == 0) {
      myReturn = 887;
    } else if (CompareUOM(name, "gev") == 0) {
      myReturn = 550;
    } else if (CompareUOM(name, "gf") == 0) {
      myReturn = 551;
    } else if (CompareUOM(name, "ghz") == 0) {
      myReturn = 552;
    } else if (CompareUOM(name, "giga standard cubic metres 15c") == 0) {
      myReturn = 567;
    } else if (CompareUOM(name, "gigabecquerel") == 0) {
      myReturn = 549;
    } else if (CompareUOM(name, "gigahertz") == 0) {
      myReturn = 552;
    } else if (CompareUOM(name, "gigajoule") == 0) {
      myReturn = 553;
    } else if (CompareUOM(name, "gigaohm") == 0) {
      myReturn = 555;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "gigapascal") == 0) {
      myReturn = 557;
    } else if (CompareUOM(name, "gigapascal per centimeter") == 0) {
      myReturn = 558;
    } else if (CompareUOM(name, "gigapascal squared") == 0) {
      myReturn = 559;
    } else if (CompareUOM(name, "gigaradian") == 0) {
      myReturn = 561;
    } else if (CompareUOM(name, "gigasiemens") == 0) {
      myReturn = 566;
    } else if (CompareUOM(name, "gigawatt") == 0) {
      myReturn = 568;
    } else if (CompareUOM(name, "gigawatt hour") == 0) {
      myReturn = 569;
    } else if (CompareUOM(name, "gigayears") == 0) {
      myReturn = 504;
    } else if (CompareUOM(name, "gj") == 0) {
      myReturn = 553;
    } else if (CompareUOM(name, "gn") == 0) {
      myReturn = 554;
    } else if (CompareUOM(name, "gohm") == 0) {
      myReturn = 555;
    } else if (CompareUOM(name, "gold coast foot") == 0) {
      myReturn = 485;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "gons") == 0) {
      myReturn = 556;
    } else if (CompareUOM(name, "gpa") == 0) {
      myReturn = 557;
    } else if (CompareUOM(name, "gpa/cm") == 0) {
      myReturn = 558;
    } else if (CompareUOM(name, "gpa2") == 0) {
      myReturn = 559;
    } else if (CompareUOM(name, "grad") == 0) {
      myReturn = 561;
    } else if (CompareUOM(name, "grain") == 0) {
      myReturn = 562;
    } else if (CompareUOM(name, "grain/ft3(100)") == 0) {
      myReturn = 563;
    } else if (CompareUOM(name, "grains/100 cubic feet") == 0) {
      myReturn = 563;
    } else if (CompareUOM(name, "grains/cubic foot") == 0) {
      myReturn = 564;
    } else if (CompareUOM(name, "grains/us gallon") == 0) {
      myReturn = 565;
    } else if (CompareUOM(name, "gram") == 0) {
      myReturn = 493;
    } else if (CompareUOM(name, "gram feet/cubic centimetre second") == 0) {
      myReturn = 494;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "gram force") == 0) {
      myReturn = 551;
    } else if (CompareUOM(name, "grams/centimetre fourth") == 0) {
      myReturn = 496;
    } else if (CompareUOM(name, "grams/cubic centimetre") == 0) {
      myReturn = 495;
    } else if (CompareUOM(name, "grams/cubic decimetre") == 0) {
      myReturn = 497;
    } else if (CompareUOM(name, "grams/cubic metre") == 0) {
      myReturn = 502;
    } else if (CompareUOM(name, "grams/kilogram") == 0) {
      myReturn = 500;
    } else if (CompareUOM(name, "grams/litre") == 0) {
      myReturn = 501;
    } else if (CompareUOM(name, "grams/second") == 0) {
      myReturn = 503;
    } else if (CompareUOM(name, "grams/uk gallon") == 0) {
      myReturn = 498;
    } else if (CompareUOM(name, "grams/us gallon") == 0) {
      myReturn = 499;
    } else if (CompareUOM(name, "gravity") == 0) {
      myReturn = 554;
    } else if (CompareUOM(name, "gray") == 0) {
      myReturn = 37;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "gs") == 0) {
      myReturn = 566;
    } else if (CompareUOM(name, "gw") == 0) {
      myReturn = 568;
    } else if (CompareUOM(name, "gw.h") == 0) {
      myReturn = 569;
    } else if (CompareUOM(name, "gy") == 0) {
      myReturn = 37;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_h(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "h") == 0) {
    myReturn = 570;
  } else if (CompareUOM(name, "h/ft3") == 0) {
    myReturn = 571;
  } else if (CompareUOM(name, "h/km") == 0) {
    myReturn = 573;
  } else if (CompareUOM(name, "h/m") == 0) {
    myReturn = 39;
  } else if (CompareUOM(name, "h/m3") == 0) {
    myReturn = 574;
  } else if (CompareUOM(name, "ha") == 0) {
    myReturn = 575;
  } else if (CompareUOM(name, "ha.m") == 0) {
    myReturn = 576;
  } else if (CompareUOM(name, "half a millisecond") == 0) {
    myReturn = 982;
  } else if (CompareUOM(name, "hbar") == 0) {
    myReturn = 577;
  } else if (CompareUOM(name, "hectare") == 0) {
    myReturn = 575;
  } else if (CompareUOM(name, "hectare metres") == 0) {
    myReturn = 576;
  } else if (CompareUOM(name, "hectobar") == 0) {
    myReturn = 577;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "hectoliter") == 0) {
      myReturn = 580;
    } else if (CompareUOM(name, "henries/metre") == 0) {
      myReturn = 39;
    } else if (CompareUOM(name, "henry") == 0) {
      myReturn = 38;
    } else if (CompareUOM(name, "hertz") == 0) {
      myReturn = 40;
    } else if (CompareUOM(name, "hhp") == 0) {
      myReturn = 578;
    } else if (CompareUOM(name, "hhp/in2") == 0) {
      myReturn = 579;
    } else if (CompareUOM(name, "hl") == 0) {
      myReturn = 580;
    } else if (CompareUOM(name, "horsepower") == 0) {
      myReturn = 581;
    } else if (CompareUOM(name, "horsepower hour") == 0) {
      myReturn = 582;
    } else if (CompareUOM(name, "horsepower hours/barrel") == 0) {
      myReturn = 583;
    } else if (CompareUOM(name, "horsepower hours/pound mass") == 0) {
      myReturn = 584;
    } else if (CompareUOM(name, "horsepower per square inch") == 0) {
      myReturn = 586;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "horsepower/cubic foot") == 0) {
      myReturn = 585;
    } else if (CompareUOM(name, "hour") == 0) {
      myReturn = 570;
    } else if (CompareUOM(name, "hour per cubic meter") == 0) {
      myReturn = 574;
    } else if (CompareUOM(name, "hour per kilometer") == 0) {
      myReturn = 573;
    } else if (CompareUOM(name, "hour per thousand foot") == 0) {
      myReturn = 572;
    } else if (CompareUOM(name, "hours/cubic foot") == 0) {
      myReturn = 571;
    } else if (CompareUOM(name, "hp") == 0) {
      myReturn = 581;
    } else if (CompareUOM(name, "hp.h") == 0) {
      myReturn = 582;
    } else if (CompareUOM(name, "hp.h/bbl") == 0) {
      myReturn = 583;
    } else if (CompareUOM(name, "hp.h/lbm") == 0) {
      myReturn = 584;
    } else if (CompareUOM(name, "hp/ft3") == 0) {
      myReturn = 585;
    } else if (CompareUOM(name, "hp/in2") == 0) {
      myReturn = 586;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "hundred seconds") == 0) {
      myReturn = 587;
    } else if (CompareUOM(name, "hydraulic horsepower") == 0) {
      myReturn = 578;
    } else if (CompareUOM(name, "hz") == 0) {
      myReturn = 40;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_i(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "imperial foot") == 0) {
    myReturn = 484;
  } else if (CompareUOM(name, "imperial yard") == 0) {
    myReturn = 1224;
  } else if (CompareUOM(name, "in") == 0) {
    myReturn = 588;
  } else if (CompareUOM(name, "in/1000") == 0) {
    myReturn = 902;
  } else if (CompareUOM(name, "in/a") == 0) {
    myReturn = 593;
  } else if (CompareUOM(name, "in/in.degf") == 0) {
    myReturn = 594;
  } else if (CompareUOM(name, "in/min") == 0) {
    myReturn = 595;
  } else if (CompareUOM(name, "in/s") == 0) {
    myReturn = 596;
  } else if (CompareUOM(name, "in2/ft2") == 0) {
    myReturn = 598;
  } else if (CompareUOM(name, "in2/in2") == 0) {
    myReturn = 599;
  } else if (CompareUOM(name, "in2/s") == 0) {
    myReturn = 600;
  } else if (CompareUOM(name, "in2|sq in") == 0) {
    myReturn = 597;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "in3") == 0) {
      myReturn = 338;
    } else if (CompareUOM(name, "in3/ft") == 0) {
      myReturn = 602;
    } else if (CompareUOM(name, "in3|cu in") == 0) {
      myReturn = 601;
    } else if (CompareUOM(name, "in4") == 0) {
      myReturn = 603;
    } else if (CompareUOM(name, "inch") == 0) {
      myReturn = 588;
    } else if (CompareUOM(name, "inch pounds force") == 0) {
      myReturn = 739;
    } else if (CompareUOM(name, "inches") == 0) {
      myReturn = 588;
    } else if (CompareUOM(name, "inches of mercury at 32 deg f") == 0) {
      myReturn = 606;
    } else if (CompareUOM(name, "inches of mercury at 60 deg f") == 0) {
      myReturn = 607;
    } else if (CompareUOM(name, "inches of water at 39.2 deg f") == 0) {
      myReturn = 604;
    } else if (CompareUOM(name, "inches of water at 60 deg f") == 0) {
      myReturn = 605;
    } else if (CompareUOM(name, "inches to the fourth") == 0) {
      myReturn = 603;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "inches/inch degree fahrenheit") == 0) {
      myReturn = 594;
    } else if (CompareUOM(name, "inches/minute") == 0) {
      myReturn = 595;
    } else if (CompareUOM(name, "inches/second") == 0) {
      myReturn = 596;
    } else if (CompareUOM(name, "inches/year") == 0) {
      myReturn = 593;
    } else if (CompareUOM(name, "indian foot") == 0) {
      myReturn = 486;
    } else if (CompareUOM(name, "indian foot, 1937") == 0) {
      myReturn = 487;
    } else if (CompareUOM(name, "indian foot, 1962") == 0) {
      myReturn = 488;
    } else if (CompareUOM(name, "indian foot, 1975") == 0) {
      myReturn = 489;
    } else if (CompareUOM(name, "indian yard") == 0) {
      myReturn = 1225;
    } else if (CompareUOM(name, "indian yard (1937)") == 0) {
      myReturn = 1226;
    } else if (CompareUOM(name, "indian yard (1962)") == 0) {
      myReturn = 1227;
    } else if (CompareUOM(name, "indian yard (1975)") == 0) {
      myReturn = 1228;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "inus") == 0) {
      myReturn = 608;
    } else if (CompareUOM(name, "inverse henry") == 0) {
      myReturn = 0;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_j(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "j") == 0) {
    myReturn = 41;
  } else if (CompareUOM(name, "j/cm2") == 0) {
    myReturn = 609;
  } else if (CompareUOM(name, "j/dm3") == 0) {
    myReturn = 610;
  } else if (CompareUOM(name, "j/g") == 0) {
    myReturn = 611;
  } else if (CompareUOM(name, "j/g.k") == 0) {
    myReturn = 612;
  } else if (CompareUOM(name, "j/k") == 0) {
    myReturn = 42;
  } else if (CompareUOM(name, "j/kg") == 0) {
    myReturn = 43;
  } else if (CompareUOM(name, "j/kg.k") == 0) {
    myReturn = 44;
  } else if (CompareUOM(name, "j/m") == 0) {
    myReturn = 613;
  } else if (CompareUOM(name, "j/m2") == 0) {
    myReturn = 614;
  } else if (CompareUOM(name, "j/m3") == 0) {
    myReturn = 45;
  } else if (CompareUOM(name, "j/mol") == 0) {
    myReturn = 46;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "j/mol.k") == 0) {
      myReturn = 47;
    } else if (CompareUOM(name, "j/s.m2.degc") == 0) {
      myReturn = 615;
    } else if (CompareUOM(name, "joule") == 0) {
      myReturn = 41;
    } else if (CompareUOM(name, "joules per delta kelvin") == 0) {
      myReturn = 42;
    } else if (CompareUOM(name, "joules/cubic decimetre") == 0) {
      myReturn = 610;
    } else if (CompareUOM(name, "joules/cubic metre") == 0) {
      myReturn = 45;
    } else if (CompareUOM(name, "joules/gram") == 0) {
      myReturn = 611;
    } else if (CompareUOM(name, "joules/gram degree kelvin") == 0) {
      myReturn = 612;
    } else if (CompareUOM(name, "joules/kilogram") == 0) {
      myReturn = 43;
    } else if (CompareUOM(name, "joules/kilogram degree kelvin") == 0) {
      myReturn = 44;
    } else if (CompareUOM(name, "joules/metre") == 0) {
      myReturn = 613;
    } else if (CompareUOM(name, "joules/mole") == 0) {
      myReturn = 46;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "joules/mole degree kelvin") == 0) {
      myReturn = 47;
    } else if (CompareUOM(name, "joules/second square metre deg c") == 0) {
      myReturn = 615;
    } else if (CompareUOM(name, "joules/square centimetre") == 0) {
      myReturn = 609;
    } else if (CompareUOM(name, "joules/square metre") == 0) {
      myReturn = 614;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_k(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "k") == 0) {
    myReturn = 363;
  } else if (CompareUOM(name, "k.m2/kw") == 0) {
    myReturn = 616;
  } else if (CompareUOM(name, "k.m2/w") == 0) {
    myReturn = 49;
  } else if (CompareUOM(name, "k/m") == 0) {
    myReturn = 50;
  } else if (CompareUOM(name, "k/s") == 0) {
    myReturn = 51;
  } else if (CompareUOM(name, "k/w") == 0) {
    myReturn = 52;
  } else if (CompareUOM(name, "ka") == 0) {
    myReturn = 617;
  } else if (CompareUOM(name, "kbbl/d") == 0) {
    myReturn = 851;
  } else if (CompareUOM(name, "kc") == 0) {
    myReturn = 620;
  } else if (CompareUOM(name, "kcal") == 0) {
    myReturn = 621;
  } else if (CompareUOM(name, "kcal.m/cm2") == 0) {
    myReturn = 622;
  } else if (CompareUOM(name, "kcal/cm3") == 0) {
    myReturn = 623;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kcal/g") == 0) {
      myReturn = 624;
    } else if (CompareUOM(name, "kcal/h") == 0) {
      myReturn = 625;
    } else if (CompareUOM(name, "kcal/h.m.degc") == 0) {
      myReturn = 626;
    } else if (CompareUOM(name, "kcal/h.m2.degc") == 0) {
      myReturn = 627;
    } else if (CompareUOM(name, "kcal/kg") == 0) {
      myReturn = 628;
    } else if (CompareUOM(name, "kcal/kg.degc") == 0) {
      myReturn = 629;
    } else if (CompareUOM(name, "kcal/m3") == 0) {
      myReturn = 630;
    } else if (CompareUOM(name, "kcd") == 0) {
      myReturn = 632;
    } else if (CompareUOM(name, "kelvin") == 0) {
      myReturn = 48;
    } else if (CompareUOM(name, "kelvin metres squared/watt") == 0) {
      myReturn = 49;
    } else if (CompareUOM(name, "kelvin per second") == 0) {
      myReturn = 51;
    } else if (CompareUOM(name, "kev") == 0) {
      myReturn = 635;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kg") == 0) {
      myReturn = 53;
    } else if (CompareUOM(name, "kg.m") == 0) {
      myReturn = 54;
    } else if (CompareUOM(name, "kg.m/cm2") == 0) {
      myReturn = 639;
    } else if (CompareUOM(name, "kg.m/s") == 0) {
      myReturn = 55;
    } else if (CompareUOM(name, "kg.m2") == 0) {
      myReturn = 56;
    } else if (CompareUOM(name, "kg/d") == 0) {
      myReturn = 640;
    } else if (CompareUOM(name, "kg/dm3") == 0) {
      myReturn = 646;
    } else if (CompareUOM(name, "kg/dm4") == 0) {
      myReturn = 642;
    } else if (CompareUOM(name, "kg/h") == 0) {
      myReturn = 643;
    } else if (CompareUOM(name, "kg/j") == 0) {
      myReturn = 57;
    } else if (CompareUOM(name, "kg/kg") == 0) {
      myReturn = 644;
    } else if (CompareUOM(name, "kg/kw.h") == 0) {
      myReturn = 645;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kg/l") == 0) {
      myReturn = 646;
    } else if (CompareUOM(name, "kg/m") == 0) {
      myReturn = 58;
    } else if (CompareUOM(name, "kg/m.s") == 0) {
      myReturn = 647;
    } else if (CompareUOM(name, "kg/m2") == 0) {
      myReturn = 59;
    } else if (CompareUOM(name, "kg/m2.s") == 0) {
      myReturn = 60;
    } else if (CompareUOM(name, "kg/m3") == 0) {
      myReturn = 61;
    } else if (CompareUOM(name, "kg/m4") == 0) {
      myReturn = 62;
    } else if (CompareUOM(name, "kg/min") == 0) {
      myReturn = 648;
    } else if (CompareUOM(name, "kg/mj") == 0) {
      myReturn = 649;
    } else if (CompareUOM(name, "kg/s") == 0) {
      myReturn = 63;
    } else if (CompareUOM(name, "kgf") == 0) {
      myReturn = 651;
    } else if (CompareUOM(name, "kgf.m") == 0) {
      myReturn = 652;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kgf.m/cm2") == 0) {
      myReturn = 653;
    } else if (CompareUOM(name, "kgf.m/m") == 0) {
      myReturn = 654;
    } else if (CompareUOM(name, "kgf.m2") == 0) {
      myReturn = 655;
    } else if (CompareUOM(name, "kgf.s/m2") == 0) {
      myReturn = 656;
    } else if (CompareUOM(name, "kgf/cm") == 0) {
      myReturn = 657;
    } else if (CompareUOM(name, "kgf/cm2") == 0) {
      myReturn = 658;
    } else if (CompareUOM(name, "kgf/kgf") == 0) {
      myReturn = 659;
    } else if (CompareUOM(name, "kgf/mm2") == 0) {
      myReturn = 660;
    } else if (CompareUOM(name, "khz") == 0) {
      myReturn = 661;
    } else if (CompareUOM(name, "kilkodynes") == 0) {
      myReturn = 633;
    } else if (CompareUOM(name, "killowatts/cubic metre degree kelvin") == 0) {
      myReturn = 719;
    } else if (CompareUOM(name, "kilo ohm metre") == 0) {
      myReturn = 691;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kilo standard cubic metres 15c") == 0) {
      myReturn = 705;
    } else if (CompareUOM(name, "kiloampere") == 0) {
      myReturn = 617;
    } else if (CompareUOM(name, "kilobyte") == 0) {
      myReturn = 619;
    } else if (CompareUOM(name, "kilocalorie metres/square centimetre") == 0) {
      myReturn = 622;
    } else if (CompareUOM(name, "kilocalorie/hour square metre deg c") == 0) {
      myReturn = 627;
    } else if (CompareUOM(name, "kilocalories") == 0) {
      myReturn = 621;
    } else if (CompareUOM(name, "kilocalories/cubic centimetre") == 0) {
      myReturn = 623;
    } else if (CompareUOM(name, "kilocalories/gram") == 0) {
      myReturn = 624;
    } else if (CompareUOM(name, "kilocalories/hour") == 0) {
      myReturn = 625;
    } else if (CompareUOM(name, "kilocalories/hour metre degree celsius") == 0) {
      myReturn = 626;
    } else if (CompareUOM(name, "kilocalories/kilogram") == 0) {
      myReturn = 628;
    } else if (CompareUOM(name, "kilocalories/kilogram degree celsius") == 0) {
      myReturn = 629;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kilocalories/mole (gram)") == 0) {
      myReturn = 631;
    } else if (CompareUOM(name, "kilocalroies/cubic metre") == 0) {
      myReturn = 630;
    } else if (CompareUOM(name, "kilocandela") == 0) {
      myReturn = 632;
    } else if (CompareUOM(name, "kilocoulombs") == 0) {
      myReturn = 620;
    } else if (CompareUOM(name, "kiloelectron volts") == 0) {
      myReturn = 635;
    } else if (CompareUOM(name, "kilogram") == 0) {
      myReturn = 53;
    } else if (CompareUOM(name, "kilogram force") == 0) {
      myReturn = 651;
    } else if (CompareUOM(name, "kilogram force metres") == 0) {
      myReturn = 652;
    } else if (CompareUOM(name, "kilogram force metres squared") == 0) {
      myReturn = 655;
    } else if (CompareUOM(name, "kilogram force metres/square centimetre") == 0) {
      myReturn = 653;
    } else if (CompareUOM(name, "kilogram force per kilogram force") == 0) {
      myReturn = 659;
    } else if (CompareUOM(name, "kilogram force/square millimetre") == 0) {
      myReturn = 660;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kilogram metres squared") == 0) {
      myReturn = 56;
    } else if (CompareUOM(name, "kilogram metres/second") == 0) {
      myReturn = 55;
    } else if (CompareUOM(name, "kilogram metres/square centimetre") == 0) {
      myReturn = 639;
    } else if (CompareUOM(name, "kilogram per 94 pound sack") == 0) {
      myReturn = 650;
    } else if (CompareUOM(name, "kilogram per day") == 0) {
      myReturn = 640;
    } else if (CompareUOM(name, "kilogram per litre") == 0) {
      myReturn = 646;
    } else if (CompareUOM(name, "kilogram per min") == 0) {
      myReturn = 648;
    } else if (CompareUOM(name, "kilogram per square centimeter") == 0) {
      myReturn = 658;
    } else if (CompareUOM(name, "kilogram/metre fourth") == 0) {
      myReturn = 62;
    } else if (CompareUOM(name, "kilograms force metres/metre") == 0) {
      myReturn = 654;
    } else if (CompareUOM(name, "kilograms force seconds/square metre") == 0) {
      myReturn = 656;
    } else if (CompareUOM(name, "kilograms force/centimetre") == 0) {
      myReturn = 657;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kilograms/cubic decimetre") == 0) {
      myReturn = 641;
    } else if (CompareUOM(name, "kilograms/cubic metre") == 0) {
      myReturn = 61;
    } else if (CompareUOM(name, "kilograms/decimetre fourth") == 0) {
      myReturn = 642;
    } else if (CompareUOM(name, "kilograms/hour") == 0) {
      myReturn = 643;
    } else if (CompareUOM(name, "kilograms/joule") == 0) {
      myReturn = 57;
    } else if (CompareUOM(name, "kilograms/kilogram") == 0) {
      myReturn = 644;
    } else if (CompareUOM(name, "kilograms/kilowatt hour") == 0) {
      myReturn = 645;
    } else if (CompareUOM(name, "kilograms/megajoule") == 0) {
      myReturn = 649;
    } else if (CompareUOM(name, "kilograms/metre") == 0) {
      myReturn = 58;
    } else if (CompareUOM(name, "kilograms/metre second") == 0) {
      myReturn = 647;
    } else if (CompareUOM(name, "kilograms/second") == 0) {
      myReturn = 63;
    } else if (CompareUOM(name, "kilograms/square metre") == 0) {
      myReturn = 59;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kilograms/square metre seconds") == 0) {
      myReturn = 60;
    } else if (CompareUOM(name, "kilohertz") == 0) {
      myReturn = 661;
    } else if (CompareUOM(name, "kilohm") == 0) {
      myReturn = 690;
    } else if (CompareUOM(name, "kilojoule metres/hour sq metre deg k") == 0) {
      myReturn = 663;
    } else if (CompareUOM(name, "kilojoule/cubic metre") == 0) {
      myReturn = 668;
    } else if (CompareUOM(name, "kilojoule/kilogram") == 0) {
      myReturn = 666;
    } else if (CompareUOM(name, "kilojoule/mole (kilogram)") == 0) {
      myReturn = 669;
    } else if (CompareUOM(name, "kilojoules") == 0) {
      myReturn = 662;
    } else if (CompareUOM(name, "kilojoules/cubic decimetre") == 0) {
      myReturn = 664;
    } else if (CompareUOM(name, "kilojoules/hour square metre deg k") == 0) {
      myReturn = 665;
    } else if (CompareUOM(name, "kilojoules/kilogram degree kelvin") == 0) {
      myReturn = 667;
    } else if (CompareUOM(name, "kilojoules/mole (kilogram) deg k") == 0) {
      myReturn = 670;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kilolux") == 0) {
      myReturn = 674;
    } else if (CompareUOM(name, "kilometer per second") == 0) {
      myReturn = 680;
    } else if (CompareUOM(name, "kilometre") == 0) {
      myReturn = 675;
    } else if (CompareUOM(name, "kilometre/ centimetre") == 0) {
      myReturn = 676;
    } else if (CompareUOM(name, "kilometres/cubic decimetre") == 0) {
      myReturn = 677;
    } else if (CompareUOM(name, "kilometres/hour") == 0) {
      myReturn = 678;
    } else if (CompareUOM(name, "kilometres/litre") == 0) {
      myReturn = 679;
    } else if (CompareUOM(name, "kilomole") == 0) {
      myReturn = 683;
    } else if (CompareUOM(name, "kilonewton metres") == 0) {
      myReturn = 685;
    } else if (CompareUOM(name, "kilonewton metres squared") == 0) {
      myReturn = 686;
    } else if (CompareUOM(name, "kilonewtons") == 0) {
      myReturn = 684;
    } else if (CompareUOM(name, "kilonewtons/metre") == 0) {
      myReturn = 687;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kilonewtons/square metre") == 0) {
      myReturn = 688;
    } else if (CompareUOM(name, "kilopascal per hour") == 0) {
      myReturn = 695;
    } else if (CompareUOM(name, "kilopascal per hundred meter") == 0) {
      myReturn = 694;
    } else if (CompareUOM(name, "kilopascal per min") == 0) {
      myReturn = 697;
    } else if (CompareUOM(name, "kilopascal seconds/metre") == 0) {
      myReturn = 693;
    } else if (CompareUOM(name, "kilopascal squared") == 0) {
      myReturn = 698;
    } else if (CompareUOM(name, "kilopascal squared per centipoise") == 0) {
      myReturn = 699;
    } else if (CompareUOM(name, "kilopascal squared per thousand centipoise") == 0) {
      myReturn = 700;
    } else if (CompareUOM(name, "kilopascals") == 0) {
      myReturn = 692;
    } else if (CompareUOM(name, "kilopascals/metre") == 0) {
      myReturn = 696;
    } else if (CompareUOM(name, "kiloradian") == 0) {
      myReturn = 703;
    } else if (CompareUOM(name, "kilosiemens") == 0) {
      myReturn = 704;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kilovolt") == 0) {
      myReturn = 708;
    } else if (CompareUOM(name, "kilowatt hours") == 0) {
      myReturn = 710;
    } else if (CompareUOM(name, "kilowatt hours/cubic metres") == 0) {
      myReturn = 714;
    } else if (CompareUOM(name, "kilowatt hours/decimetre") == 0) {
      myReturn = 711;
    } else if (CompareUOM(name, "kilowatt hours/kilogram") == 0) {
      myReturn = 712;
    } else if (CompareUOM(name, "kilowatt hours/kilogram degree c") == 0) {
      myReturn = 713;
    } else if (CompareUOM(name, "kilowatts") == 0) {
      myReturn = 709;
    } else if (CompareUOM(name, "kilowatts/cubic metre") == 0) {
      myReturn = 718;
    } else if (CompareUOM(name, "kilowatts/square centimetre") == 0) {
      myReturn = 715;
    } else if (CompareUOM(name, "kilowatts/square metre") == 0) {
      myReturn = 716;
    } else if (CompareUOM(name, "kilowatts/square metre degree kelvin") == 0) {
      myReturn = 717;
    } else if (CompareUOM(name, "kj") == 0) {
      myReturn = 662;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kj.m/h.m2.k") == 0) {
      myReturn = 663;
    } else if (CompareUOM(name, "kj/dm3") == 0) {
      myReturn = 664;
    } else if (CompareUOM(name, "kj/h.m2.k") == 0) {
      myReturn = 665;
    } else if (CompareUOM(name, "kj/kg") == 0) {
      myReturn = 666;
    } else if (CompareUOM(name, "kj/kg.k") == 0) {
      myReturn = 667;
    } else if (CompareUOM(name, "kj/kmol.k") == 0) {
      myReturn = 670;
    } else if (CompareUOM(name, "kj/m3") == 0) {
      myReturn = 668;
    } else if (CompareUOM(name, "kj/mol") == 0) {
      myReturn = 669;
    } else if (CompareUOM(name, "klx") == 0) {
      myReturn = 674;
    } else if (CompareUOM(name, "km") == 0) {
      myReturn = 675;
    } else if (CompareUOM(name, "km/cm") == 0) {
      myReturn = 676;
    } else if (CompareUOM(name, "km/dm3") == 0) {
      myReturn = 677;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "km/h") == 0) {
      myReturn = 678;
    } else if (CompareUOM(name, "km/l") == 0) {
      myReturn = 679;
    } else if (CompareUOM(name, "km/s") == 0) {
      myReturn = 680;
    } else if (CompareUOM(name, "km2") == 0) {
      myReturn = 681;
    } else if (CompareUOM(name, "km3") == 0) {
      myReturn = 682;
    } else if (CompareUOM(name, "km3/(d.m)") == 0) {
      myReturn = 191;
    } else if (CompareUOM(name, "km3/(h.m)") == 0) {
      myReturn = 193;
    } else if (CompareUOM(name, "kmol") == 0) {
      myReturn = 683;
    } else if (CompareUOM(name, "kn") == 0) {
      myReturn = 684;
    } else if (CompareUOM(name, "kn.m") == 0) {
      myReturn = 685;
    } else if (CompareUOM(name, "kn.m2") == 0) {
      myReturn = 686;
    } else if (CompareUOM(name, "kn/m") == 0) {
      myReturn = 687;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kn/m2") == 0) {
      myReturn = 688;
    } else if (CompareUOM(name, "knots") == 0) {
      myReturn = 689;
    } else if (CompareUOM(name, "kohm") == 0) {
      myReturn = 690;
    } else if (CompareUOM(name, "kohm.m") == 0) {
      myReturn = 691;
    } else if (CompareUOM(name, "kpa") == 0) {
      myReturn = 692;
    } else if (CompareUOM(name, "kpa.s/m") == 0) {
      myReturn = 693;
    } else if (CompareUOM(name, "kpa/h") == 0) {
      myReturn = 695;
    } else if (CompareUOM(name, "kpa/hm") == 0) {
      myReturn = 694;
    } else if (CompareUOM(name, "kpa/m") == 0) {
      myReturn = 696;
    } else if (CompareUOM(name, "kpa/min") == 0) {
      myReturn = 697;
    } else if (CompareUOM(name, "kpa2") == 0) {
      myReturn = 698;
    } else if (CompareUOM(name, "kpa2/cp") == 0) {
      myReturn = 699;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "ks") == 0) {
      myReturn = 704;
    } else if (CompareUOM(name, "kv") == 0) {
      myReturn = 708;
    } else if (CompareUOM(name, "kw") == 0) {
      myReturn = 709;
    } else if (CompareUOM(name, "kw.h") == 0) {
      myReturn = 710;
    } else if (CompareUOM(name, "kw.h/dm3") == 0) {
      myReturn = 711;
    } else if (CompareUOM(name, "kw.h/kg") == 0) {
      myReturn = 712;
    } else if (CompareUOM(name, "kw.h/kg.degc") == 0) {
      myReturn = 713;
    } else if (CompareUOM(name, "kw.h/m3") == 0) {
      myReturn = 714;
    } else if (CompareUOM(name, "kw/cm2") == 0) {
      myReturn = 715;
    } else if (CompareUOM(name, "kw/m2") == 0) {
      myReturn = 716;
    } else if (CompareUOM(name, "kw/m2.k") == 0) {
      myReturn = 717;
    } else if (CompareUOM(name, "kw/m3") == 0) {
      myReturn = 718;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "kw/m3.k") == 0) {
      myReturn = 719;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_l(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "l") == 0) {
    myReturn = 720;
  } else if (CompareUOM(name, "l/h") == 0) {
    myReturn = 725;
  } else if (CompareUOM(name, "l/kg") == 0) {
    myReturn = 726;
  } else if (CompareUOM(name, "l/km(100)") == 0) {
    myReturn = 722;
  } else if (CompareUOM(name, "l/m") == 0) {
    myReturn = 727;
  } else if (CompareUOM(name, "l/m3") == 0) {
    myReturn = 728;
  } else if (CompareUOM(name, "l/min") == 0) {
    myReturn = 729;
  } else if (CompareUOM(name, "l/mol") == 0) {
    myReturn = 731;
  } else if (CompareUOM(name, "l/s") == 0) {
    myReturn = 732;
  } else if (CompareUOM(name, "l/s2") == 0) {
    myReturn = 733;
  } else if (CompareUOM(name, "l/t") == 0) {
    myReturn = 734;
  } else if (CompareUOM(name, "l/tonuk") == 0) {
    myReturn = 735;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "lbf") == 0) {
      myReturn = 736;
    } else if (CompareUOM(name, "lbf.ft/in") == 0) {
      myReturn = 737;
    } else if (CompareUOM(name, "lbf.ft/in2") == 0) {
      myReturn = 738;
    } else if (CompareUOM(name, "lbf.in") == 0) {
      myReturn = 739;
    } else if (CompareUOM(name, "lbf.in/in") == 0) {
      myReturn = 740;
    } else if (CompareUOM(name, "lbf.in2") == 0) {
      myReturn = 741;
    } else if (CompareUOM(name, "lbf.s/ft2") == 0) {
      myReturn = 742;
    } else if (CompareUOM(name, "lbf.s/in2") == 0) {
      myReturn = 743;
    } else if (CompareUOM(name, "lbf/ft") == 0) {
      myReturn = 747;
    } else if (CompareUOM(name, "lbf/ft2") == 0) {
      myReturn = 1061;
    } else if (CompareUOM(name, "lbf/ft2(100)") == 0) {
      myReturn = 745;
    } else if (CompareUOM(name, "lbf/ft3") == 0) {
      myReturn = 1061;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "lbf/galus") == 0) {
      myReturn = 750;
    } else if (CompareUOM(name, "lbf/in") == 0) {
      myReturn = 751;
    } else if (CompareUOM(name, "lbf/in2") == 0) {
      myReturn = 1062;
    } else if (CompareUOM(name, "lbf/lbf") == 0) {
      myReturn = 753;
    } else if (CompareUOM(name, "lbm") == 0) {
      myReturn = 754;
    } else if (CompareUOM(name, "lbm.ft/s") == 0) {
      myReturn = 755;
    } else if (CompareUOM(name, "lbm.ft2") == 0) {
      myReturn = 756;
    } else if (CompareUOM(name, "lbm.ft2/s2") == 0) {
      myReturn = 757;
    } else if (CompareUOM(name, "lbm/bbl") == 0) {
      myReturn = 763;
    } else if (CompareUOM(name, "lbm/d") == 0) {
      myReturn = 764;
    } else if (CompareUOM(name, "lbm/ft") == 0) {
      myReturn = 765;
    } else if (CompareUOM(name, "lbm/ft.h") == 0) {
      myReturn = 776;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "lbm/ft.s") == 0) {
      myReturn = 783;
    } else if (CompareUOM(name, "lbm/ft2") == 0) {
      myReturn = 768;
    } else if (CompareUOM(name, "lbm/ft3") == 0) {
      myReturn = 769;
    } else if (CompareUOM(name, "lbm/ft4") == 0) {
      myReturn = 770;
    } else if (CompareUOM(name, "lbm/galuk") == 0) {
      myReturn = 771;
    } else if (CompareUOM(name, "lbm/galuk(1000)") == 0) {
      myReturn = 759;
    } else if (CompareUOM(name, "lbm/galuk.ft") == 0) {
      myReturn = 772;
    } else if (CompareUOM(name, "lbm/galus") == 0) {
      myReturn = 773;
    } else if (CompareUOM(name, "lbm/galus(1000)") == 0) {
      myReturn = 760;
    } else if (CompareUOM(name, "lbm/galus.ft") == 0) {
      myReturn = 774;
    } else if (CompareUOM(name, "lbm/h") == 0) {
      myReturn = 775;
    } else if (CompareUOM(name, "lbm/h.ft") == 0) {
      myReturn = 776;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "lbm/h.ft2") == 0) {
      myReturn = 777;
    } else if (CompareUOM(name, "lbm/hp.h") == 0) {
      myReturn = 778;
    } else if (CompareUOM(name, "lbm/in3") == 0) {
      myReturn = 779;
    } else if (CompareUOM(name, "lbm/min") == 0) {
      myReturn = 781;
    } else if (CompareUOM(name, "lbm/s") == 0) {
      myReturn = 782;
    } else if (CompareUOM(name, "lbm/s.ft") == 0) {
      myReturn = 783;
    } else if (CompareUOM(name, "lbm/s.ft2") == 0) {
      myReturn = 784;
    } else if (CompareUOM(name, "liter per hour") == 0) {
      myReturn = 725;
    } else if (CompareUOM(name, "liter per hundred kilogram") == 0) {
      myReturn = 721;
    } else if (CompareUOM(name, "liter per kilogram") == 0) {
      myReturn = 726;
    } else if (CompareUOM(name, "liter per minute") == 0) {
      myReturn = 729;
    } else if (CompareUOM(name, "liter per ten barrel") == 0) {
      myReturn = 723;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "liter per uk ton") == 0) {
      myReturn = 735;
    } else if (CompareUOM(name, "litre") == 0) {
      myReturn = 720;
    } else if (CompareUOM(name, "litres per minute per bar") == 0) {
      myReturn = 724;
    } else if (CompareUOM(name, "litres/100 kilometres") == 0) {
      myReturn = 722;
    } else if (CompareUOM(name, "litres/cubic metre") == 0) {
      myReturn = 728;
    } else if (CompareUOM(name, "litres/metre") == 0) {
      myReturn = 727;
    } else if (CompareUOM(name, "litres/mole (gram)") == 0) {
      myReturn = 730;
    } else if (CompareUOM(name, "litres/mole (kilogram)") == 0) {
      myReturn = 731;
    } else if (CompareUOM(name, "litres/second") == 0) {
      myReturn = 732;
    } else if (CompareUOM(name, "litres/second/second") == 0) {
      myReturn = 733;
    } else if (CompareUOM(name, "litres/tonne") == 0) {
      myReturn = 734;
    } else if (CompareUOM(name, "lm") == 0) {
      myReturn = 64;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "lm.s") == 0) {
      myReturn = 1129;
    } else if (CompareUOM(name, "lm/m2") == 0) {
      myReturn = 790;
    } else if (CompareUOM(name, "lm/w") == 0) {
      myReturn = 66;
    } else if (CompareUOM(name, "lumen") == 0) {
      myReturn = 64;
    } else if (CompareUOM(name, "lumen second") == 0) {
      myReturn = 65;
    } else if (CompareUOM(name, "lumens/square metre") == 0) {
      myReturn = 790;
    } else if (CompareUOM(name, "lumens/watt") == 0) {
      myReturn = 66;
    } else if (CompareUOM(name, "lux") == 0) {
      myReturn = 67;
    } else if (CompareUOM(name, "lux seconds") == 0) {
      myReturn = 68;
    } else if (CompareUOM(name, "lx") == 0) {
      myReturn = 67;
    } else if (CompareUOM(name, "lx.s") == 0) {
      myReturn = 68;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_m(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "m") == 0) {
    myReturn = 69;
  } else if (CompareUOM(name, "m/cm") == 0) {
    myReturn = 797;
  } else if (CompareUOM(name, "m/d") == 0) {
    myReturn = 798;
  } else if (CompareUOM(name, "m/h") == 0) {
    myReturn = 799;
  } else if (CompareUOM(name, "m/k") == 0) {
    myReturn = 70;
  } else if (CompareUOM(name, "m/km") == 0) {
    myReturn = 800;
  } else if (CompareUOM(name, "m/m") == 0) {
    myReturn = 801;
  } else if (CompareUOM(name, "m/m.k") == 0) {
    myReturn = 802;
  } else if (CompareUOM(name, "m/m3") == 0) {
    myReturn = 803;
  } else if (CompareUOM(name, "m/min") == 0) {
    myReturn = 804;
  } else if (CompareUOM(name, "m/ms") == 0) {
    myReturn = 805;
  } else if (CompareUOM(name, "m/s") == 0) {
    myReturn = 71;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "m/s2") == 0) {
      myReturn = 72;
    } else if (CompareUOM(name, "m2") == 0) {
      myReturn = 73;
    } else if (CompareUOM(name, "m2/cm3") == 0) {
      myReturn = 806;
    } else if (CompareUOM(name, "m2/d.kpa") == 0) {
      myReturn = 807;
    } else if (CompareUOM(name, "m2/g") == 0) {
      myReturn = 808;
    } else if (CompareUOM(name, "m2/h") == 0) {
      myReturn = 809;
    } else if (CompareUOM(name, "m2/kg") == 0) {
      myReturn = 74;
    } else if (CompareUOM(name, "m2/m2") == 0) {
      myReturn = 810;
    } else if (CompareUOM(name, "m2/m3") == 0) {
      myReturn = 811;
    } else if (CompareUOM(name, "m2/mol") == 0) {
      myReturn = 75;
    } else if (CompareUOM(name, "m2/pa.s") == 0) {
      myReturn = 76;
    } else if (CompareUOM(name, "m2/s") == 0) {
      myReturn = 77;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "m3") == 0) {
      myReturn = 78;
    } else if (CompareUOM(name, "m3.bar.h") == 0) {
      myReturn = 815;
    } else if (CompareUOM(name, "m3/(d.bar)") == 0) {
      myReturn = 814;
    } else if (CompareUOM(name, "m3/(d.kpa)") == 0) {
      myReturn = 828;
    } else if (CompareUOM(name, "m3/(d.m)") == 0) {
      myReturn = 821;
    } else if (CompareUOM(name, "m3/(d.psi)") == 0) {
      myReturn = 836;
    } else if (CompareUOM(name, "m3/(h.bar)") == 0) {
      myReturn = 815;
    } else if (CompareUOM(name, "m3/(h.m)") == 0) {
      myReturn = 825;
    } else if (CompareUOM(name, "m3/(min.bar)") == 0) {
      myReturn = 816;
    } else if (CompareUOM(name, "m3/(s.ft)") == 0) {
      myReturn = 837;
    } else if (CompareUOM(name, "m3/bar.d") == 0) {
      myReturn = 814;
    } else if (CompareUOM(name, "m3/bar.h") == 0) {
      myReturn = 815;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "m3/bar.min") == 0) {
      myReturn = 816;
    } else if (CompareUOM(name, "m3/cp.d.kpa") == 0) {
      myReturn = 817;
    } else if (CompareUOM(name, "m3/cp.pa.s") == 0) {
      myReturn = 818;
    } else if (CompareUOM(name, "m3/d") == 0) {
      myReturn = 819;
    } else if (CompareUOM(name, "m3/d.kpa") == 0) {
      myReturn = 828;
    } else if (CompareUOM(name, "m3/d.m") == 0) {
      myReturn = 821;
    } else if (CompareUOM(name, "m3/d2") == 0) {
      myReturn = 822;
    } else if (CompareUOM(name, "m3/g") == 0) {
      myReturn = 823;
    } else if (CompareUOM(name, "m3/h") == 0) {
      myReturn = 824;
    } else if (CompareUOM(name, "m3/h.m") == 0) {
      myReturn = 825;
    } else if (CompareUOM(name, "m3/ha.m") == 0) {
      myReturn = 826;
    } else if (CompareUOM(name, "m3/j") == 0) {
      myReturn = 79;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "m3/k.kpa") == 0) {
      myReturn = 828;
    } else if (CompareUOM(name, "m3/kg") == 0) {
      myReturn = 80;
    } else if (CompareUOM(name, "m3/km") == 0) {
      myReturn = 827;
    } else if (CompareUOM(name, "m3/kpa.d") == 0) {
      myReturn = 828;
    } else if (CompareUOM(name, "m3/kpa.h") == 0) {
      myReturn = 829;
    } else if (CompareUOM(name, "m3/kw.h") == 0) {
      myReturn = 830;
    } else if (CompareUOM(name, "m3/m") == 0) {
      myReturn = 831;
    } else if (CompareUOM(name, "m3/m2") == 0) {
      myReturn = 832;
    } else if (CompareUOM(name, "m3/m3") == 0) {
      myReturn = 833;
    } else if (CompareUOM(name, "m3/min") == 0) {
      myReturn = 834;
    } else if (CompareUOM(name, "m3/mol") == 0) {
      myReturn = 835;
    } else if (CompareUOM(name, "m3/mol(kg)") == 0) {
      myReturn = 835;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "m3/pa.s") == 0) {
      myReturn = 83;
    } else if (CompareUOM(name, "m3/pa/s") == 0) {
      myReturn = 83;
    } else if (CompareUOM(name, "m3/pa2.s2") == 0) {
      myReturn = 84;
    } else if (CompareUOM(name, "m3/psi.d") == 0) {
      myReturn = 836;
    } else if (CompareUOM(name, "m3/s") == 0) {
      myReturn = 85;
    } else if (CompareUOM(name, "m3/s.ft") == 0) {
      myReturn = 837;
    } else if (CompareUOM(name, "m3/s.m") == 0) {
      myReturn = 838;
    } else if (CompareUOM(name, "m3/s.m2") == 0) {
      myReturn = 839;
    } else if (CompareUOM(name, "m3/s2") == 0) {
      myReturn = 86;
    } else if (CompareUOM(name, "m3/t") == 0) {
      myReturn = 840;
    } else if (CompareUOM(name, "m3/tonuk") == 0) {
      myReturn = 841;
    } else if (CompareUOM(name, "m3/tonus") == 0) {
      myReturn = 842;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "m4") == 0) {
      myReturn = 89;
    } else if (CompareUOM(name, "m4/s") == 0) {
      myReturn = 90;
    } else if (CompareUOM(name, "ma") == 0) {
      myReturn = 845;
    } else if (CompareUOM(name, "ma/cm2") == 0) {
      myReturn = 846;
    } else if (CompareUOM(name, "ma/ft2") == 0) {
      myReturn = 847;
    } else if (CompareUOM(name, "mbar") == 0) {
      myReturn = 848;
    } else if (CompareUOM(name, "mbbl") == 0) {
      myReturn = 849;
    } else if (CompareUOM(name, "mbbl.ft/d") == 0) {
      myReturn = 850;
    } else if (CompareUOM(name, "mbbl/d") == 0) {
      myReturn = 618;
    } else if (CompareUOM(name, "mbq") == 0) {
      myReturn = 852;
    } else if (CompareUOM(name, "mbyte") == 0) {
      myReturn = 853;
    } else if (CompareUOM(name, "mc") == 0) {
      myReturn = 854;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "mc/m2") == 0) {
      myReturn = 855;
    } else if (CompareUOM(name, "mci") == 0) {
      myReturn = 856;
    } else if (CompareUOM(name, "md") == 0) {
      myReturn = 857;
    } else if (CompareUOM(name, "md.ft") == 0) {
      myReturn = 858;
    } else if (CompareUOM(name, "md.ft2/lbf.s") == 0) {
      myReturn = 859;
    } else if (CompareUOM(name, "md.in2/lbf.s") == 0) {
      myReturn = 860;
    } else if (CompareUOM(name, "md.m") == 0) {
      myReturn = 861;
    } else if (CompareUOM(name, "md/cp") == 0) {
      myReturn = 862;
    } else if (CompareUOM(name, "md/pa.s") == 0) {
      myReturn = 863;
    } else if (CompareUOM(name, "mega pounds per square inch") == 0) {
      myReturn = 975;
    } else if (CompareUOM(name, "mega standard cubic metres 15c") == 0) {
      myReturn = 994;
    } else if (CompareUOM(name, "megaampere") == 0) {
      myReturn = 845;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "megabecquerel") == 0) {
      myReturn = 852;
    } else if (CompareUOM(name, "megabyte") == 0) {
      myReturn = 853;
    } else if (CompareUOM(name, "megaflops") == 0) {
      myReturn = 870;
    } else if (CompareUOM(name, "megagram") == 0) {
      myReturn = 871;
    } else if (CompareUOM(name, "megagrams/day") == 0) {
      myReturn = 874;
    } else if (CompareUOM(name, "megagrams/hour") == 0) {
      myReturn = 877;
    } else if (CompareUOM(name, "megagrams/square metre") == 0) {
      myReturn = 882;
    } else if (CompareUOM(name, "megagrams/year") == 0) {
      myReturn = 873;
    } else if (CompareUOM(name, "megahertz") == 0) {
      myReturn = 894;
    } else if (CompareUOM(name, "megajoules") == 0) {
      myReturn = 911;
    } else if (CompareUOM(name, "megajoules/cubic metre") == 0) {
      myReturn = 918;
    } else if (CompareUOM(name, "megajoules/kilogram") == 0) {
      myReturn = 915;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "megajoules/metre") == 0) {
      myReturn = 916;
    } else if (CompareUOM(name, "megajoules/mole (kilogram)") == 0) {
      myReturn = 919;
    } else if (CompareUOM(name, "megajoules/year") == 0) {
      myReturn = 913;
    } else if (CompareUOM(name, "megameter") == 0) {
      myReturn = 926;
    } else if (CompareUOM(name, "meganewtons") == 0) {
      myReturn = 949;
    } else if (CompareUOM(name, "megaohm") == 0) {
      myReturn = 954;
    } else if (CompareUOM(name, "megapascal per hour") == 0) {
      myReturn = 973;
    } else if (CompareUOM(name, "megapascal per meter") == 0) {
      myReturn = 974;
    } else if (CompareUOM(name, "megapascal seconds/metre (megarayl)") == 0) {
      myReturn = 972;
    } else if (CompareUOM(name, "megapascals") == 0) {
      myReturn = 970;
    } else if (CompareUOM(name, "megaradian") == 0) {
      myReturn = 977;
    } else if (CompareUOM(name, "megavolt") == 0) {
      myReturn = 1000;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "megawatt hours") == 0) {
      myReturn = 1006;
    } else if (CompareUOM(name, "megawatt hours/cubic metre") == 0) {
      myReturn = 1008;
    } else if (CompareUOM(name, "megawatt hours/kilogram") == 0) {
      myReturn = 1007;
    } else if (CompareUOM(name, "megawatts") == 0) {
      myReturn = 1004;
    } else if (CompareUOM(name, "megayears") == 0) {
      myReturn = 844;
    } else if (CompareUOM(name, "meq") == 0) {
      myReturn = 864;
    } else if (CompareUOM(name, "meq/cm3") == 0) {
      myReturn = 866;
    } else if (CompareUOM(name, "meq/g") == 0) {
      myReturn = 867;
    } else if (CompareUOM(name, "meter") == 0) {
      myReturn = 69;
    } else if (CompareUOM(name, "meter per minute") == 0) {
      myReturn = 804;
    } else if (CompareUOM(name, "meter-kilogram") == 0) {
      myReturn = 54;
    } else if (CompareUOM(name, "metre") == 0) {
      myReturn = 69;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "metres fourth") == 0) {
      myReturn = 89;
    } else if (CompareUOM(name, "metres fourth/second") == 0) {
      myReturn = 90;
    } else if (CompareUOM(name, "metres per thirty metres") == 0) {
      myReturn = 796;
    } else if (CompareUOM(name, "metres/ centimetre") == 0) {
      myReturn = 797;
    } else if (CompareUOM(name, "metres/cubic metre") == 0) {
      myReturn = 803;
    } else if (CompareUOM(name, "metres/day") == 0) {
      myReturn = 798;
    } else if (CompareUOM(name, "metres/degree kelvin") == 0) {
      myReturn = 70;
    } else if (CompareUOM(name, "metres/hour") == 0) {
      myReturn = 799;
    } else if (CompareUOM(name, "metres/kilometre") == 0) {
      myReturn = 800;
    } else if (CompareUOM(name, "metres/metre") == 0) {
      myReturn = 801;
    } else if (CompareUOM(name, "metres/metre kelvin") == 0) {
      myReturn = 802;
    } else if (CompareUOM(name, "metres/millisecond") == 0) {
      myReturn = 805;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "metres/second") == 0) {
      myReturn = 71;
    } else if (CompareUOM(name, "metres/second squared") == 0) {
      myReturn = 72;
    } else if (CompareUOM(name, "meuc") == 0) {
      myReturn = 1055;
    } else if (CompareUOM(name, "mev") == 0) {
      myReturn = 869;
    } else if (CompareUOM(name, "mg") == 0) {
      myReturn = 872;
    } else if (CompareUOM(name, "mg/a") == 0) {
      myReturn = 873;
    } else if (CompareUOM(name, "mg/d") == 0) {
      myReturn = 874;
    } else if (CompareUOM(name, "mg/dm3") == 0) {
      myReturn = 875;
    } else if (CompareUOM(name, "mg/galus") == 0) {
      myReturn = 876;
    } else if (CompareUOM(name, "mg/h") == 0) {
      myReturn = 877;
    } else if (CompareUOM(name, "mg/in") == 0) {
      myReturn = 878;
    } else if (CompareUOM(name, "mg/j") == 0) {
      myReturn = 879;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "mg/kg") == 0) {
      myReturn = 880;
    } else if (CompareUOM(name, "mg/l") == 0) {
      myReturn = 881;
    } else if (CompareUOM(name, "mg/m2") == 0) {
      myReturn = 882;
    } else if (CompareUOM(name, "mg/m3") == 0) {
      myReturn = 884;
    } else if (CompareUOM(name, "mgal") == 0) {
      myReturn = 885;
    } else if (CompareUOM(name, "mgn") == 0) {
      myReturn = 889;
    } else if (CompareUOM(name, "mgy") == 0) {
      myReturn = 890;
    } else if (CompareUOM(name, "mh") == 0) {
      myReturn = 891;
    } else if (CompareUOM(name, "mho") == 0) {
      myReturn = 892;
    } else if (CompareUOM(name, "mho/m") == 0) {
      myReturn = 893;
    } else if (CompareUOM(name, "mhos") == 0) {
      myReturn = 892;
    } else if (CompareUOM(name, "mhos/metre") == 0) {
      myReturn = 893;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "mhz") == 0) {
      myReturn = 895;
    } else if (CompareUOM(name, "mi") == 0) {
      myReturn = 896;
    } else if (CompareUOM(name, "mi/galuk") == 0) {
      myReturn = 897;
    } else if (CompareUOM(name, "mi/galus") == 0) {
      myReturn = 898;
    } else if (CompareUOM(name, "mi/h") == 0) {
      myReturn = 899;
    } else if (CompareUOM(name, "mi/in") == 0) {
      myReturn = 900;
    } else if (CompareUOM(name, "mi2") == 0) {
      myReturn = 901;
    } else if (CompareUOM(name, "mi3") == 0) {
      myReturn = 340;
    } else if (CompareUOM(name, "microampere") == 0) {
      myReturn = 1168;
    } else if (CompareUOM(name, "microampere per square centimeter") == 0) {
      myReturn = 1169;
    } else if (CompareUOM(name, "microampere per square inch") == 0) {
      myReturn = 1170;
    } else if (CompareUOM(name, "microbars") == 0) {
      myReturn = 1171;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "microcalories/second square centimetre") == 0) {
      myReturn = 1173;
    } else if (CompareUOM(name, "microcoulomb") == 0) {
      myReturn = 1172;
    } else if (CompareUOM(name, "microcurie") == 0) {
      myReturn = 1174;
    } else if (CompareUOM(name, "microeuclids") == 0) {
      myReturn = 1175;
    } else if (CompareUOM(name, "microfarads") == 0) {
      myReturn = 1176;
    } else if (CompareUOM(name, "microfarads/metre") == 0) {
      myReturn = 1177;
    } else if (CompareUOM(name, "micrograms") == 0) {
      myReturn = 1178;
    } else if (CompareUOM(name, "micrograms/cubic centimetre") == 0) {
      myReturn = 1179;
    } else if (CompareUOM(name, "microhenries/metre") == 0) {
      myReturn = 1181;
    } else if (CompareUOM(name, "microhenry") == 0) {
      myReturn = 1180;
    } else if (CompareUOM(name, "microhertz") == 0) {
      myReturn = 1182;
    } else if (CompareUOM(name, "microhm per foot") == 0) {
      myReturn = 1193;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "microhm per meter") == 0) {
      myReturn = 1194;
    } else if (CompareUOM(name, "microjoules") == 0) {
      myReturn = 1183;
    } else if (CompareUOM(name, "micrometer per second") == 0) {
      myReturn = 1185;
    } else if (CompareUOM(name, "micromole") == 0) {
      myReturn = 1189;
    } else if (CompareUOM(name, "micronewtons") == 0) {
      myReturn = 1190;
    } else if (CompareUOM(name, "microns") == 0) {
      myReturn = 1184;
    } else if (CompareUOM(name, "microns of mercury at 0 deg c") == 0) {
      myReturn = 1188;
    } else if (CompareUOM(name, "microohm") == 0) {
      myReturn = 1192;
    } else if (CompareUOM(name, "micropascal") == 0) {
      myReturn = 1195;
    } else if (CompareUOM(name, "micropounds/square inch") == 0) {
      myReturn = 1196;
    } else if (CompareUOM(name, "microradian") == 0) {
      myReturn = 1197;
    } else if (CompareUOM(name, "microsecond") == 0) {
      myReturn = 1199;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "microseconds/foot") == 0) {
      myReturn = 1200;
    } else if (CompareUOM(name, "microseconds/metre") == 0) {
      myReturn = 1201;
    } else if (CompareUOM(name, "microsiemens") == 0) {
      myReturn = 1198;
    } else if (CompareUOM(name, "microteslas") == 0) {
      myReturn = 1202;
    } else if (CompareUOM(name, "microvolt per foot") == 0) {
      myReturn = 1204;
    } else if (CompareUOM(name, "microvolt per meter") == 0) {
      myReturn = 1205;
    } else if (CompareUOM(name, "microvolts") == 0) {
      myReturn = 1203;
    } else if (CompareUOM(name, "microwatts") == 0) {
      myReturn = 1206;
    } else if (CompareUOM(name, "microwatts/cubic metre") == 0) {
      myReturn = 1207;
    } else if (CompareUOM(name, "microwebers") == 0) {
      myReturn = 1208;
    } else if (CompareUOM(name, "mil, a thousandth of an inch") == 0) {
      myReturn = 902;
    } else if (CompareUOM(name, "mil_6400") == 0) {
      myReturn = 904;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "mile") == 0) {
      myReturn = 896;
    } else if (CompareUOM(name, "miles/hour") == 0) {
      myReturn = 899;
    } else if (CompareUOM(name, "miles/inch") == 0) {
      myReturn = 900;
    } else if (CompareUOM(name, "miles/uk gallon") == 0) {
      myReturn = 897;
    } else if (CompareUOM(name, "miles/us gallon") == 0) {
      myReturn = 898;
    } else if (CompareUOM(name, "milli-rem") == 0) {
      myReturn = 978;
    } else if (CompareUOM(name, "milli-rems per hour") == 0) {
      myReturn = 979;
    } else if (CompareUOM(name, "milliamp") == 0) {
      myReturn = 843;
    } else if (CompareUOM(name, "milliampere per square centimeter") == 0) {
      myReturn = 846;
    } else if (CompareUOM(name, "milliampere per square foot") == 0) {
      myReturn = 847;
    } else if (CompareUOM(name, "millibar") == 0) {
      myReturn = 848;
    } else if (CompareUOM(name, "millicoulomb") == 0) {
      myReturn = 854;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "millicoulombs/square metre") == 0) {
      myReturn = 855;
    } else if (CompareUOM(name, "millicurie") == 0) {
      myReturn = 856;
    } else if (CompareUOM(name, "millidarcies/centipoise") == 0) {
      myReturn = 862;
    } else if (CompareUOM(name, "millidarcies/pascal second") == 0) {
      myReturn = 863;
    } else if (CompareUOM(name, "millidarcy") == 0) {
      myReturn = 857;
    } else if (CompareUOM(name, "millidarcy foot") == 0) {
      myReturn = 858;
    } else if (CompareUOM(name, "millidarcy metres") == 0) {
      myReturn = 861;
    } else if (CompareUOM(name, "millidarcy sq feet/pound force second") == 0) {
      myReturn = 859;
    } else if (CompareUOM(name, "millidarcy sq inches/pound force second") == 0) {
      myReturn = 860;
    } else if (CompareUOM(name, "millidegrees kelvin/metre") == 0) {
      myReturn = 920;
    } else if (CompareUOM(name, "milliequivalent") == 0) {
      myReturn = 864;
    } else if (CompareUOM(name, "milliequivalents/ cubic centimeter") == 0) {
      myReturn = 866;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "milliequivalents/ gram") == 0) {
      myReturn = 867;
    } else if (CompareUOM(name, "milliequivalents/ hectogram") == 0) {
      myReturn = 865;
    } else if (CompareUOM(name, "millieuclid") == 0) {
      myReturn = 868;
    } else if (CompareUOM(name, "milligalileo") == 0) {
      myReturn = 885;
    } else if (CompareUOM(name, "milligauss") == 0) {
      myReturn = 886;
    } else if (CompareUOM(name, "milligram") == 0) {
      myReturn = 872;
    } else if (CompareUOM(name, "milligram per litre") == 0) {
      myReturn = 881;
    } else if (CompareUOM(name, "milligrams/cubic decimetre") == 0) {
      myReturn = 875;
    } else if (CompareUOM(name, "milligrams/cubic metre") == 0) {
      myReturn = 883;
    } else if (CompareUOM(name, "milligrams/joule") == 0) {
      myReturn = 879;
    } else if (CompareUOM(name, "milligrams/kilogram") == 0) {
      myReturn = 880;
    } else if (CompareUOM(name, "milligrams/us gallon") == 0) {
      myReturn = 876;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "milligravity") == 0) {
      myReturn = 889;
    } else if (CompareUOM(name, "milligray") == 0) {
      myReturn = 890;
    } else if (CompareUOM(name, "millihenries") == 0) {
      myReturn = 891;
    } else if (CompareUOM(name, "millihertz") == 0) {
      myReturn = 895;
    } else if (CompareUOM(name, "millijoules") == 0) {
      myReturn = 912;
    } else if (CompareUOM(name, "millijoules/square centimetre") == 0) {
      myReturn = 914;
    } else if (CompareUOM(name, "millijoules/square metre") == 0) {
      myReturn = 917;
    } else if (CompareUOM(name, "milliliter per milliliter") == 0) {
      myReturn = 924;
    } else if (CompareUOM(name, "millilitre") == 0) {
      myReturn = 921;
    } else if (CompareUOM(name, "millilitres/uk gallon") == 0) {
      myReturn = 922;
    } else if (CompareUOM(name, "millilitres/us gallon") == 0) {
      myReturn = 923;
    } else if (CompareUOM(name, "millimetres") == 0) {
      myReturn = 925;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "millimetres of mercury at 0 deg c") == 0) {
      myReturn = 937;
    } else if (CompareUOM(name, "millimetres/millimetre degree kelvin") == 0) {
      myReturn = 928;
    } else if (CompareUOM(name, "millimetres/second") == 0) {
      myReturn = 929;
    } else if (CompareUOM(name, "millimetres/year") == 0) {
      myReturn = 927;
    } else if (CompareUOM(name, "millimhos/metre") == 0) {
      myReturn = 938;
    } else if (CompareUOM(name, "millimole") == 0) {
      myReturn = 939;
    } else if (CompareUOM(name, "millinewton metres squared") == 0) {
      myReturn = 951;
    } else if (CompareUOM(name, "millinewtons") == 0) {
      myReturn = 950;
    } else if (CompareUOM(name, "millinewtons/kilometre") == 0) {
      myReturn = 952;
    } else if (CompareUOM(name, "millinewtons/metre") == 0) {
      myReturn = 953;
    } else if (CompareUOM(name, "milliohm") == 0) {
      myReturn = 955;
    } else if (CompareUOM(name, "million barrels") == 0) {
      myReturn = 935;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "million barrels/acre foot") == 0) {
      myReturn = 936;
    } else if (CompareUOM(name, "million btus/hour") == 0) {
      myReturn = 255;
    } else if (CompareUOM(name, "million cubic feet") == 0) {
      myReturn = 791;
    } else if (CompareUOM(name, "million cubic feet per acre-foot") == 0) {
      myReturn = 792;
    } else if (CompareUOM(name, "million cubic feet per day") == 0) {
      myReturn = 793;
    } else if (CompareUOM(name, "million cubic meters") == 0) {
      myReturn = 794;
    } else if (CompareUOM(name, "million cubic metres per day") == 0) {
      myReturn = 795;
    } else if (CompareUOM(name, "million pounds mass/year") == 0) {
      myReturn = 758;
    } else if (CompareUOM(name, "million standard cubic feet at 60 deg f") == 0) {
      myReturn = 940;
    } else if (CompareUOM(name, "million standard cubic feet/day") == 0) {
      myReturn = 941;
    } else if (CompareUOM(name, "million standard cubic metres 15c") == 0) {
      myReturn = 943;
    } else if (CompareUOM(name, "million stbs, 60 deg f/acre foot") == 0) {
      myReturn = 948;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "million std cu ft/ stock tank barrel") == 0) {
      myReturn = 942;
    } else if (CompareUOM(name, "million std cubic metres, 15 degc/day") == 0) {
      myReturn = 944;
    } else if (CompareUOM(name, "million stock tank barrels 60 deg f") == 0) {
      myReturn = 945;
    } else if (CompareUOM(name, "million stock tank barrels 60 deg f/acre") == 0) {
      myReturn = 947;
    } else if (CompareUOM(name, "million stock tank barrels, 60 deg f/day") == 0) {
      myReturn = 946;
    } else if (CompareUOM(name, "millions of electron volts") == 0) {
      myReturn = 869;
    } else if (CompareUOM(name, "millipascal") == 0) {
      myReturn = 969;
    } else if (CompareUOM(name, "millipascal seconds") == 0) {
      myReturn = 971;
    } else if (CompareUOM(name, "milliradian") == 0) {
      myReturn = 976;
    } else if (CompareUOM(name, "millisecond per foot") == 0) {
      myReturn = 984;
    } else if (CompareUOM(name, "millisecond per meter") == 0) {
      myReturn = 987;
    } else if (CompareUOM(name, "milliseconds") == 0) {
      myReturn = 981;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "milliseconds angular") == 0) {
      myReturn = 993;
    } else if (CompareUOM(name, "milliseconds/centimetre") == 0) {
      myReturn = 983;
    } else if (CompareUOM(name, "milliseconds/inch") == 0) {
      myReturn = 985;
    } else if (CompareUOM(name, "milliseconds/second") == 0) {
      myReturn = 988;
    } else if (CompareUOM(name, "millisiemen") == 0) {
      myReturn = 980;
    } else if (CompareUOM(name, "millisiemens/metre") == 0) {
      myReturn = 986;
    } else if (CompareUOM(name, "millisievert") == 0) {
      myReturn = 997;
    } else if (CompareUOM(name, "millisieverts per hour") == 0) {
      myReturn = 998;
    } else if (CompareUOM(name, "milliteslas") == 0) {
      myReturn = 999;
    } else if (CompareUOM(name, "millivolt per foot") == 0) {
      myReturn = 1002;
    } else if (CompareUOM(name, "millivolt per meter") == 0) {
      myReturn = 1003;
    } else if (CompareUOM(name, "millivolts") == 0) {
      myReturn = 1001;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "milliwatt") == 0) {
      myReturn = 1005;
    } else if (CompareUOM(name, "milliwatts/square metres") == 0) {
      myReturn = 1009;
    } else if (CompareUOM(name, "milliwebers") == 0) {
      myReturn = 1010;
    } else if (CompareUOM(name, "mils/year") == 0) {
      myReturn = 903;
    } else if (CompareUOM(name, "min") == 0) {
      myReturn = 905;
    } else if (CompareUOM(name, "min/ft") == 0) {
      myReturn = 906;
    } else if (CompareUOM(name, "min/m") == 0) {
      myReturn = 907;
    } else if (CompareUOM(name, "mina") == 0) {
      myReturn = 908;
    } else if (CompareUOM(name, "minute per foot") == 0) {
      myReturn = 906;
    } else if (CompareUOM(name, "minute per meter") == 0) {
      myReturn = 907;
    } else if (CompareUOM(name, "minutes") == 0) {
      myReturn = 905;
    } else if (CompareUOM(name, "minutes angular") == 0) {
      myReturn = 908;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "mius") == 0) {
      myReturn = 909;
    } else if (CompareUOM(name, "mius2") == 0) {
      myReturn = 910;
    } else if (CompareUOM(name, "mj") == 0) {
      myReturn = 912;
    } else if (CompareUOM(name, "mj/a") == 0) {
      myReturn = 913;
    } else if (CompareUOM(name, "mj/cm2") == 0) {
      myReturn = 914;
    } else if (CompareUOM(name, "mj/kg") == 0) {
      myReturn = 915;
    } else if (CompareUOM(name, "mj/m") == 0) {
      myReturn = 916;
    } else if (CompareUOM(name, "mj/m2") == 0) {
      myReturn = 917;
    } else if (CompareUOM(name, "mj/m3") == 0) {
      myReturn = 918;
    } else if (CompareUOM(name, "mj/mol") == 0) {
      myReturn = 919;
    } else if (CompareUOM(name, "ml") == 0) {
      myReturn = 921;
    } else if (CompareUOM(name, "ml/galuk") == 0) {
      myReturn = 922;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "ml/galus") == 0) {
      myReturn = 923;
    } else if (CompareUOM(name, "ml/ml") == 0) {
      myReturn = 924;
    } else if (CompareUOM(name, "mlbm/yr") == 0) {
      myReturn = 758;
    } else if (CompareUOM(name, "mm") == 0) {
      myReturn = 926;
    } else if (CompareUOM(name, "mm/a") == 0) {
      myReturn = 927;
    } else if (CompareUOM(name, "mm/mm.k") == 0) {
      myReturn = 928;
    } else if (CompareUOM(name, "mm/s") == 0) {
      myReturn = 929;
    } else if (CompareUOM(name, "mm2") == 0) {
      myReturn = 930;
    } else if (CompareUOM(name, "mm2/mm2") == 0) {
      myReturn = 931;
    } else if (CompareUOM(name, "mm2/s") == 0) {
      myReturn = 932;
    } else if (CompareUOM(name, "mm3") == 0) {
      myReturn = 933;
    } else if (CompareUOM(name, "mm3/j") == 0) {
      myReturn = 934;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "mmho/m") == 0) {
      myReturn = 938;
    } else if (CompareUOM(name, "mmol") == 0) {
      myReturn = 939;
    } else if (CompareUOM(name, "mmscm(15c)") == 0) {
      myReturn = 994;
    } else if (CompareUOM(name, "mn") == 0) {
      myReturn = 950;
    } else if (CompareUOM(name, "mn.m2") == 0) {
      myReturn = 951;
    } else if (CompareUOM(name, "mn/km") == 0) {
      myReturn = 952;
    } else if (CompareUOM(name, "mn/m") == 0) {
      myReturn = 953;
    } else if (CompareUOM(name, "modified american foot") == 0) {
      myReturn = 490;
    } else if (CompareUOM(name, "mohm") == 0) {
      myReturn = 955;
    } else if (CompareUOM(name, "mol") == 0) {
      myReturn = 957;
    } else if (CompareUOM(name, "mol/h") == 0) {
      myReturn = 958;
    } else if (CompareUOM(name, "mol/m2") == 0) {
      myReturn = 92;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "mol/m2.s") == 0) {
      myReturn = 93;
    } else if (CompareUOM(name, "mol/m3") == 0) {
      myReturn = 959;
    } else if (CompareUOM(name, "mol/s") == 0) {
      myReturn = 960;
    } else if (CompareUOM(name, "mole") == 0) {
      myReturn = 91;
    } else if (CompareUOM(name, "mole (gram)") == 0) {
      myReturn = 956;
    } else if (CompareUOM(name, "mole (kilogram)") == 0) {
      myReturn = 957;
    } else if (CompareUOM(name, "moles (kilogram)/cubic metre") == 0) {
      myReturn = 959;
    } else if (CompareUOM(name, "moles (kilogram)/hour") == 0) {
      myReturn = 958;
    } else if (CompareUOM(name, "moles (kilogram)/second") == 0) {
      myReturn = 960;
    } else if (CompareUOM(name, "moles (pounds mass)") == 0) {
      myReturn = 961;
    } else if (CompareUOM(name, "moles (pounds mass)/cubic foot") == 0) {
      myReturn = 962;
    } else if (CompareUOM(name, "moles (pounds mass)/hour") == 0) {
      myReturn = 965;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "moles (pounds mass)/hour square foot") == 0) {
      myReturn = 966;
    } else if (CompareUOM(name, "moles (pounds mass)/second") == 0) {
      myReturn = 967;
    } else if (CompareUOM(name, "moles (pounds mass)/second square foot") == 0) {
      myReturn = 968;
    } else if (CompareUOM(name, "moles (pounds mass)/uk gallon") == 0) {
      myReturn = 963;
    } else if (CompareUOM(name, "moles (pounds mass)/us gallon") == 0) {
      myReturn = 964;
    } else if (CompareUOM(name, "moles/cubic metre") == 0) {
      myReturn = 94;
    } else if (CompareUOM(name, "moles/second") == 0) {
      myReturn = 95;
    } else if (CompareUOM(name, "moles/square metre") == 0) {
      myReturn = 92;
    } else if (CompareUOM(name, "moles/square metre second") == 0) {
      myReturn = 93;
    } else if (CompareUOM(name, "mpa") == 0) {
      myReturn = 970;
    } else if (CompareUOM(name, "mpa.s") == 0) {
      myReturn = 971;
    } else if (CompareUOM(name, "mpa.s/m") == 0) {
      myReturn = 972;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "mpa/h") == 0) {
      myReturn = 973;
    } else if (CompareUOM(name, "mpa/m") == 0) {
      myReturn = 974;
    } else if (CompareUOM(name, "mpsi") == 0) {
      myReturn = 975;
    } else if (CompareUOM(name, "mrad") == 0) {
      myReturn = 977;
    } else if (CompareUOM(name, "ms") == 0) {
      myReturn = 981;
    } else if (CompareUOM(name, "ms/cm") == 0) {
      myReturn = 983;
    } else if (CompareUOM(name, "ms/ft") == 0) {
      myReturn = 984;
    } else if (CompareUOM(name, "ms/in") == 0) {
      myReturn = 985;
    } else if (CompareUOM(name, "ms/m") == 0) {
      myReturn = 987;
    } else if (CompareUOM(name, "ms/s") == 0) {
      myReturn = 988;
    } else if (CompareUOM(name, "mscm3") == 0) {
      myReturn = 943;
    } else if (CompareUOM(name, "msv") == 0) {
      myReturn = 997;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "mt") == 0) {
      myReturn = 999;
    } else if (CompareUOM(name, "mv") == 0) {
      myReturn = 1001;
    } else if (CompareUOM(name, "mv/ft") == 0) {
      myReturn = 1002;
    } else if (CompareUOM(name, "mv/m") == 0) {
      myReturn = 1003;
    } else if (CompareUOM(name, "mw") == 0) {
      myReturn = 1005;
    } else if (CompareUOM(name, "mw.h") == 0) {
      myReturn = 1006;
    } else if (CompareUOM(name, "mw.h/kg") == 0) {
      myReturn = 1007;
    } else if (CompareUOM(name, "mw.h/m3") == 0) {
      myReturn = 1008;
    } else if (CompareUOM(name, "mw/m2") == 0) {
      myReturn = 1009;
    } else if (CompareUOM(name, "mwb") == 0) {
      myReturn = 1010;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_n(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "n") == 0) {
    myReturn = 96;
  } else if (CompareUOM(name, "n.m") == 0) {
    myReturn = 1011;
  } else if (CompareUOM(name, "n.m/m") == 0) {
    myReturn = 1012;
  } else if (CompareUOM(name, "n.m2") == 0) {
    myReturn = 97;
  } else if (CompareUOM(name, "n.s/m2") == 0) {
    myReturn = 1013;
  } else if (CompareUOM(name, "n/m") == 0) {
    myReturn = 98;
  } else if (CompareUOM(name, "n/m2") == 0) {
    myReturn = 1015;
  } else if (CompareUOM(name, "n/m3") == 0) {
    myReturn = 99;
  } else if (CompareUOM(name, "n/mm2") == 0) {
    myReturn = 1016;
  } else if (CompareUOM(name, "n4/kg.m7") == 0) {
    myReturn = 100;
  } else if (CompareUOM(name, "na") == 0) {
    myReturn = 1017;
  } else if (CompareUOM(name, "nanoampere") == 0) {
    myReturn = 1017;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "nanocoulomb") == 0) {
      myReturn = 1019;
    } else if (CompareUOM(name, "nanocurie") == 0) {
      myReturn = 1020;
    } else if (CompareUOM(name, "nanoeuclid") == 0) {
      myReturn = 1021;
    } else if (CompareUOM(name, "nanohenry") == 0) {
      myReturn = 1022;
    } else if (CompareUOM(name, "nanohm") == 0) {
      myReturn = 1026;
    } else if (CompareUOM(name, "nanojoules") == 0) {
      myReturn = 1023;
    } else if (CompareUOM(name, "nanometer per second") == 0) {
      myReturn = 1025;
    } else if (CompareUOM(name, "nanometres") == 0) {
      myReturn = 1024;
    } else if (CompareUOM(name, "nanoseconds") == 0) {
      myReturn = 1027;
    } else if (CompareUOM(name, "nanoseconds/foot") == 0) {
      myReturn = 1028;
    } else if (CompareUOM(name, "nanoseconds/metre") == 0) {
      myReturn = 1029;
    } else if (CompareUOM(name, "nanoteslas") == 0) {
      myReturn = 1030;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "nanowatts") == 0) {
      myReturn = 1031;
    } else if (CompareUOM(name, "napi") == 0) {
      myReturn = 101;
    } else if (CompareUOM(name, "nautical mile") == 0) {
      myReturn = 1018;
    } else if (CompareUOM(name, "nc") == 0) {
      myReturn = 1019;
    } else if (CompareUOM(name, "nci") == 0) {
      myReturn = 1020;
    } else if (CompareUOM(name, "newton") == 0) {
      myReturn = 96;
    } else if (CompareUOM(name, "newton metre") == 0) {
      myReturn = 1011;
    } else if (CompareUOM(name, "newton metres/metre") == 0) {
      myReturn = 1012;
    } else if (CompareUOM(name, "newton per thirty meters") == 0) {
      myReturn = 1014;
    } else if (CompareUOM(name, "newton seconds/metre squared") == 0) {
      myReturn = 1013;
    } else if (CompareUOM(name, "newton square metres") == 0) {
      myReturn = 97;
    } else if (CompareUOM(name, "newton/metre fourth/kilogram metre cubed") == 0) {
      myReturn = 412;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "newtons fourth metres/kilogram") == 0) {
      myReturn = 100;
    } else if (CompareUOM(name, "newtons/cubic metre") == 0) {
      myReturn = 99;
    } else if (CompareUOM(name, "newtons/metre") == 0) {
      myReturn = 98;
    } else if (CompareUOM(name, "newtons/square metre") == 0) {
      myReturn = 1015;
    } else if (CompareUOM(name, "newtons/square millimetre") == 0) {
      myReturn = 1016;
    } else if (CompareUOM(name, "nh") == 0) {
      myReturn = 1022;
    } else if (CompareUOM(name, "nj") == 0) {
      myReturn = 1023;
    } else if (CompareUOM(name, "nm") == 0) {
      myReturn = 1024;
    } else if (CompareUOM(name, "nm/s") == 0) {
      myReturn = 1025;
    } else if (CompareUOM(name, "nohm") == 0) {
      myReturn = 1026;
    } else if (CompareUOM(name, "ns") == 0) {
      myReturn = 1027;
    } else if (CompareUOM(name, "ns/ft") == 0) {
      myReturn = 1028;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "ns/m") == 0) {
      myReturn = 1029;
    } else if (CompareUOM(name, "nt") == 0) {
      myReturn = 1030;
    } else if (CompareUOM(name, "nw") == 0) {
      myReturn = 1031;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_o(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "o") == 0) {
    myReturn = 102;
  } else if (CompareUOM(name, "octave") == 0) {
    myReturn = 102;
  } else if (CompareUOM(name, "oe") == 0) {
    myReturn = 1032;
  } else if (CompareUOM(name, "oersted") == 0) {
    myReturn = 1032;
  } else if (CompareUOM(name, "ohm") == 0) {
    myReturn = 103;
  } else if (CompareUOM(name, "ohm centimetres") == 0) {
    myReturn = 1033;
  } else if (CompareUOM(name, "ohm metre") == 0) {
    myReturn = 104;
  } else if (CompareUOM(name, "ohm per metre") == 0) {
    myReturn = 105;
  } else if (CompareUOM(name, "ohm.cm") == 0) {
    myReturn = 1033;
  } else if (CompareUOM(name, "ohm.m") == 0) {
    myReturn = 104;
  } else if (CompareUOM(name, "ohm/m") == 0) {
    myReturn = 105;
  } else if (CompareUOM(name, "ounce force") == 0) {
    myReturn = 1036;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "ounce mass") == 0) {
      myReturn = 1037;
    } else if (CompareUOM(name, "ozf") == 0) {
      myReturn = 1036;
    } else if (CompareUOM(name, "ozm") == 0) {
      myReturn = 1037;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_p(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "p") == 0) {
    myReturn = 1038;
  } else if (CompareUOM(name, "pa") == 0) {
    myReturn = 1039;
  } else if (CompareUOM(name, "pa.s") == 0) {
    myReturn = 107;
  } else if (CompareUOM(name, "pa.s/m3") == 0) {
    myReturn = 108;
  } else if (CompareUOM(name, "pa.s/m6") == 0) {
    myReturn = 109;
  } else if (CompareUOM(name, "pa.s2/m3") == 0) {
    myReturn = 1040;
  } else if (CompareUOM(name, "pa/h") == 0) {
    myReturn = 1042;
  } else if (CompareUOM(name, "pa/m") == 0) {
    myReturn = 110;
  } else if (CompareUOM(name, "pa/m3") == 0) {
    myReturn = 111;
  } else if (CompareUOM(name, "pa/s") == 0) {
    myReturn = 112;
  } else if (CompareUOM(name, "pa2") == 0) {
    myReturn = 113;
  } else if (CompareUOM(name, "part per million per degree celsius") == 0) {
    myReturn = 1057;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "part per million per degree fahrenheit") == 0) {
      myReturn = 1058;
    } else if (CompareUOM(name, "parts per million") == 0) {
      myReturn = 1056;
    } else if (CompareUOM(name, "parts per ten thousand") == 0) {
      myReturn = 1054;
    } else if (CompareUOM(name, "parts per thousand") == 0) {
      myReturn = 1055;
    } else if (CompareUOM(name, "pascal") == 0) {
      myReturn = 106;
    } else if (CompareUOM(name, "pascal gauge") == 0) {
      myReturn = 1041;
    } else if (CompareUOM(name, "pascal per hour") == 0) {
      myReturn = 1042;
    } else if (CompareUOM(name, "pascal second /cubic metre squared") == 0) {
      myReturn = 109;
    } else if (CompareUOM(name, "pascal seconds") == 0) {
      myReturn = 107;
    } else if (CompareUOM(name, "pascal seconds squared/ cubic metre") == 0) {
      myReturn = 1040;
    } else if (CompareUOM(name, "pascal seconds/cubic metre") == 0) {
      myReturn = 108;
    } else if (CompareUOM(name, "pascal squared") == 0) {
      myReturn = 113;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "pascal/ second") == 0) {
      myReturn = 112;
    } else if (CompareUOM(name, "pascals/cubic metre") == 0) {
      myReturn = 111;
    } else if (CompareUOM(name, "pascals/metre") == 0) {
      myReturn = 110;
    } else if (CompareUOM(name, "pc") == 0) {
      myReturn = 1043;
    } else if (CompareUOM(name, "pci") == 0) {
      myReturn = 1045;
    } else if (CompareUOM(name, "pci/g") == 0) {
      myReturn = 1044;
    } else if (CompareUOM(name, "per angstrom") == 0) {
      myReturn = 153;
    } else if (CompareUOM(name, "per annum") == 0) {
      myReturn = 152;
    } else if (CompareUOM(name, "per bar") == 0) {
      myReturn = 154;
    } else if (CompareUOM(name, "per barrel") == 0) {
      myReturn = 155;
    } else if (CompareUOM(name, "per centimetre") == 0) {
      myReturn = 156;
    } else if (CompareUOM(name, "per cubic foot") == 0) {
      myReturn = 163;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "per cubic metre") == 0) {
      myReturn = 5;
    } else if (CompareUOM(name, "per day") == 0) {
      myReturn = 157;
    } else if (CompareUOM(name, "per degree celsius") == 0) {
      myReturn = 158;
    } else if (CompareUOM(name, "per degree fahrenheit") == 0) {
      myReturn = 159;
    } else if (CompareUOM(name, "per degree rankine") == 0) {
      myReturn = 160;
    } else if (CompareUOM(name, "per foot") == 0) {
      myReturn = 161;
    } else if (CompareUOM(name, "per gram") == 0) {
      myReturn = 164;
    } else if (CompareUOM(name, "per hour") == 0) {
      myReturn = 167;
    } else if (CompareUOM(name, "per inch") == 0) {
      myReturn = 168;
    } else if (CompareUOM(name, "per kelvin") == 0) {
      myReturn = 1;
    } else if (CompareUOM(name, "per kilogram") == 0) {
      myReturn = 2;
    } else if (CompareUOM(name, "per kilopascal") == 0) {
      myReturn = 170;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "per litre") == 0) {
      myReturn = 171;
    } else if (CompareUOM(name, "per metre") == 0) {
      myReturn = 3;
    } else if (CompareUOM(name, "per micro pounds per square inch") == 0) {
      myReturn = 181;
    } else if (CompareUOM(name, "per microvolt") == 0) {
      myReturn = 182;
    } else if (CompareUOM(name, "per mile") == 0) {
      myReturn = 174;
    } else if (CompareUOM(name, "per mille") == 0) {
      myReturn = 1050;
    } else if (CompareUOM(name, "per millimetre") == 0) {
      myReturn = 177;
    } else if (CompareUOM(name, "per minute") == 0) {
      myReturn = 176;
    } else if (CompareUOM(name, "per nanometre") == 0) {
      myReturn = 178;
    } else if (CompareUOM(name, "per newton") == 0) {
      myReturn = 6;
    } else if (CompareUOM(name, "per pascal") == 0) {
      myReturn = 7;
    } else if (CompareUOM(name, "per pico pascal") == 0) {
      myReturn = 179;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "per pound") == 0) {
      myReturn = 173;
    } else if (CompareUOM(name, "per pound force") == 0) {
      myReturn = 172;
    } else if (CompareUOM(name, "per pounds/square inch") == 0) {
      myReturn = 180;
    } else if (CompareUOM(name, "per second") == 0) {
      myReturn = 8;
    } else if (CompareUOM(name, "per square foot") == 0) {
      myReturn = 162;
    } else if (CompareUOM(name, "per square kilometre") == 0) {
      myReturn = 169;
    } else if (CompareUOM(name, "per square metre") == 0) {
      myReturn = 4;
    } else if (CompareUOM(name, "per square mile") == 0) {
      myReturn = 175;
    } else if (CompareUOM(name, "per uk gallon") == 0) {
      myReturn = 165;
    } else if (CompareUOM(name, "per us gallon") == 0) {
      myReturn = 166;
    } else if (CompareUOM(name, "per volt") == 0) {
      myReturn = 9;
    } else if (CompareUOM(name, "per week") == 0) {
      myReturn = 183;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "per yard") == 0) {
      myReturn = 184;
    } else if (CompareUOM(name, "percent") == 0) {
      myReturn = 151;
    } else if (CompareUOM(name, "permil") == 0) {
      myReturn = 1055;
    } else if (CompareUOM(name, "pf") == 0) {
      myReturn = 1051;
    } else if (CompareUOM(name, "ph") == 0) {
      myReturn = 114;
    } else if (CompareUOM(name, "picoampere") == 0) {
      myReturn = 1039;
    } else if (CompareUOM(name, "picocoulomb") == 0) {
      myReturn = 1043;
    } else if (CompareUOM(name, "picocurie") == 0) {
      myReturn = 1045;
    } else if (CompareUOM(name, "picocurie per gram") == 0) {
      myReturn = 1044;
    } else if (CompareUOM(name, "picometer") == 0) {
      myReturn = 1052;
    } else if (CompareUOM(name, "picopascal") == 0) {
      myReturn = 1053;
    } else if (CompareUOM(name, "picosecond") == 0) {
      myReturn = 1059;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "picosiemens") == 0) {
      myReturn = 1060;
    } else if (CompareUOM(name, "picrofarads") == 0) {
      myReturn = 1051;
    } else if (CompareUOM(name, "pm") == 0) {
      myReturn = 1052;
    } else if (CompareUOM(name, "poise") == 0) {
      myReturn = 1038;
    } else if (CompareUOM(name, "pound force per pound force") == 0) {
      myReturn = 753;
    } else if (CompareUOM(name, "pound mass per day") == 0) {
      myReturn = 764;
    } else if (CompareUOM(name, "pound per square inch per hour") == 0) {
      myReturn = 1067;
    } else if (CompareUOM(name, "pound per square inch per meter") == 0) {
      myReturn = 1069;
    } else if (CompareUOM(name, "pound per square inch per minute") == 0) {
      myReturn = 1070;
    } else if (CompareUOM(name, "pound per square inch second") == 0) {
      myReturn = 1064;
    } else if (CompareUOM(name, "pound per square inch squared") == 0) {
      myReturn = 1071;
    } else if (CompareUOM(name, "poundal centimetre squared") == 0) {
      myReturn = 1047;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "poundals") == 0) {
      myReturn = 1046;
    } else if (CompareUOM(name, "poundals/centimetre") == 0) {
      myReturn = 1049;
    } else if (CompareUOM(name, "pounds force") == 0) {
      myReturn = 736;
    } else if (CompareUOM(name, "pounds force feet/inch") == 0) {
      myReturn = 737;
    } else if (CompareUOM(name, "pounds force inches squared") == 0) {
      myReturn = 741;
    } else if (CompareUOM(name, "pounds force inches/inch") == 0) {
      myReturn = 740;
    } else if (CompareUOM(name, "pounds force per foot") == 0) {
      myReturn = 747;
    } else if (CompareUOM(name, "pounds force per hundred foot") == 0) {
      myReturn = 744;
    } else if (CompareUOM(name, "pounds force per thirty meters") == 0) {
      myReturn = 746;
    } else if (CompareUOM(name, "pounds force seconds/square foot") == 0) {
      myReturn = 742;
    } else if (CompareUOM(name, "pounds force seconds/square inch") == 0) {
      myReturn = 743;
    } else if (CompareUOM(name, "pounds force/100 square foot") == 0) {
      myReturn = 745;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "pounds force/cubic foot") == 0) {
      myReturn = 749;
    } else if (CompareUOM(name, "pounds force/inch") == 0) {
      myReturn = 751;
    } else if (CompareUOM(name, "pounds force/square foot") == 0) {
      myReturn = 748;
    } else if (CompareUOM(name, "pounds force/square inch") == 0) {
      myReturn = 752;
    } else if (CompareUOM(name, "pounds force/us gallon") == 0) {
      myReturn = 750;
    } else if (CompareUOM(name, "pounds mass") == 0) {
      myReturn = 754;
    } else if (CompareUOM(name, "pounds mass per 10 barrel") == 0) {
      myReturn = 762;
    } else if (CompareUOM(name, "pounds mass per hundred square foot") == 0) {
      myReturn = 761;
    } else if (CompareUOM(name, "pounds mass square feet") == 0) {
      myReturn = 756;
    } else if (CompareUOM(name, "pounds mass square feet/second squared") == 0) {
      myReturn = 757;
    } else if (CompareUOM(name, "pounds mass/1000 barrels") == 0) {
      myReturn = 780;
    } else if (CompareUOM(name, "pounds mass/1000 uk gallons") == 0) {
      myReturn = 759;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "pounds mass/1000 us gallons") == 0) {
      myReturn = 760;
    } else if (CompareUOM(name, "pounds mass/barrel") == 0) {
      myReturn = 763;
    } else if (CompareUOM(name, "pounds mass/cubic foot") == 0) {
      myReturn = 769;
    } else if (CompareUOM(name, "pounds mass/cubic inch") == 0) {
      myReturn = 779;
    } else if (CompareUOM(name, "pounds mass/foot") == 0) {
      myReturn = 765;
    } else if (CompareUOM(name, "pounds mass/foot fourth") == 0) {
      myReturn = 770;
    } else if (CompareUOM(name, "pounds mass/foot hour") == 0) {
      myReturn = 766;
    } else if (CompareUOM(name, "pounds mass/foot second") == 0) {
      myReturn = 767;
    } else if (CompareUOM(name, "pounds mass/horsepower hour") == 0) {
      myReturn = 778;
    } else if (CompareUOM(name, "pounds mass/hour") == 0) {
      myReturn = 775;
    } else if (CompareUOM(name, "pounds mass/hour foot") == 0) {
      myReturn = 776;
    } else if (CompareUOM(name, "pounds mass/hour square foot") == 0) {
      myReturn = 777;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "pounds mass/minute") == 0) {
      myReturn = 781;
    } else if (CompareUOM(name, "pounds mass/second") == 0) {
      myReturn = 782;
    } else if (CompareUOM(name, "pounds mass/second foot") == 0) {
      myReturn = 783;
    } else if (CompareUOM(name, "pounds mass/second square foot") == 0) {
      myReturn = 784;
    } else if (CompareUOM(name, "pounds mass/square foot") == 0) {
      myReturn = 768;
    } else if (CompareUOM(name, "pounds mass/uk gallon") == 0) {
      myReturn = 771;
    } else if (CompareUOM(name, "pounds mass/uk gallon foot") == 0) {
      myReturn = 772;
    } else if (CompareUOM(name, "pounds mass/us gallon") == 0) {
      myReturn = 773;
    } else if (CompareUOM(name, "pounds mass/us gallon foot") == 0) {
      myReturn = 774;
    } else if (CompareUOM(name, "pounds per square inch days/barrel") == 0) {
      myReturn = 1063;
    } else if (CompareUOM(name, "pounds/square foot") == 0) {
      myReturn = 1061;
    } else if (CompareUOM(name, "pounds/square inch") == 0) {
      myReturn = 1062;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "pounds/square inch absolute") == 0) {
      myReturn = 1075;
    } else if (CompareUOM(name, "pounds/square inch gauge") == 0) {
      myReturn = 1076;
    } else if (CompareUOM(name, "pounds/square inch per 100 feet") == 0) {
      myReturn = 1065;
    } else if (CompareUOM(name, "pounds/square inch per foot") == 0) {
      myReturn = 1066;
    } else if (CompareUOM(name, "pounds/square inch per thousand feet") == 0) {
      myReturn = 1068;
    } else if (CompareUOM(name, "pounds/square inch squared/ centipoise") == 0) {
      myReturn = 1074;
    } else if (CompareUOM(name, "ppa") == 0) {
      myReturn = 1053;
    } else if (CompareUOM(name, "ppk") == 0) {
      myReturn = 1055;
    } else if (CompareUOM(name, "ppm") == 0) {
      myReturn = 1219;
    } else if (CompareUOM(name, "ppm/degc") == 0) {
      myReturn = 1057;
    } else if (CompareUOM(name, "ppm/degf") == 0) {
      myReturn = 1058;
    } else if (CompareUOM(name, "ps") == 0) {
      myReturn = 1060;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "psi") == 0) {
      myReturn = 1062;
    } else if (CompareUOM(name, "psi squared days/ centipoise cubic foot") == 0) {
      myReturn = 1072;
    } else if (CompareUOM(name, "psi.d/bbl") == 0) {
      myReturn = 1063;
    } else if (CompareUOM(name, "psi.s") == 0) {
      myReturn = 1064;
    } else if (CompareUOM(name, "psi/ft") == 0) {
      myReturn = 1066;
    } else if (CompareUOM(name, "psi/ft(100)") == 0) {
      myReturn = 1065;
    } else if (CompareUOM(name, "psi/h") == 0) {
      myReturn = 1067;
    } else if (CompareUOM(name, "psi/m") == 0) {
      myReturn = 1069;
    } else if (CompareUOM(name, "psi/min") == 0) {
      myReturn = 1070;
    } else if (CompareUOM(name, "psi2") == 0) {
      myReturn = 1071;
    } else if (CompareUOM(name, "psi2.d/cp.ft3") == 0) {
      myReturn = 1072;
    } else if (CompareUOM(name, "psi2.d2/cp.ft6") == 0) {
      myReturn = 1073;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "psi2/cp") == 0) {
      myReturn = 1074;
    } else if (CompareUOM(name, "pu") == 0) {
      myReturn = 151;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_r(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "rad") == 0) {
    myReturn = 1088;
  } else if (CompareUOM(name, "rad/ft") == 0) {
    myReturn = 1086;
  } else if (CompareUOM(name, "rad/ft3") == 0) {
    myReturn = 1087;
  } else if (CompareUOM(name, "rad/m") == 0) {
    myReturn = 116;
  } else if (CompareUOM(name, "rad/m3") == 0) {
    myReturn = 117;
  } else if (CompareUOM(name, "rad/s") == 0) {
    myReturn = 118;
  } else if (CompareUOM(name, "rad/s2") == 0) {
    myReturn = 119;
  } else if (CompareUOM(name, "radian") == 0) {
    myReturn = 115;
  } else if (CompareUOM(name, "radians per cubic foot") == 0) {
    myReturn = 1087;
  } else if (CompareUOM(name, "radians per cubic metre") == 0) {
    myReturn = 117;
  } else if (CompareUOM(name, "radians per foot") == 0) {
    myReturn = 1086;
  } else if (CompareUOM(name, "radians/metre") == 0) {
    myReturn = 116;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "radians/second") == 0) {
      myReturn = 118;
    } else if (CompareUOM(name, "radians/second squared") == 0) {
      myReturn = 119;
    } else if (CompareUOM(name, "rem") == 0) {
      myReturn = 1089;
    } else if (CompareUOM(name, "rems per hour") == 0) {
      myReturn = 1090;
    } else if (CompareUOM(name, "rev/s") == 0) {
      myReturn = 286;
    } else if (CompareUOM(name, "revolutions/minute") == 0) {
      myReturn = 1091;
    } else if (CompareUOM(name, "revolutions/minute per second") == 0) {
      myReturn = 1093;
    } else if (CompareUOM(name, "revolutions/second") == 0) {
      myReturn = 1092;
    } else if (CompareUOM(name, "rpm") == 0) {
      myReturn = 1091;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_s(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "s") == 0) {
    myReturn = 121;
  } else if (CompareUOM(name, "s/cm") == 0) {
    myReturn = 1094;
  } else if (CompareUOM(name, "s/ft") == 0) {
    myReturn = 1095;
  } else if (CompareUOM(name, "s/ft3") == 0) {
    myReturn = 1096;
  } else if (CompareUOM(name, "s/in") == 0) {
    myReturn = 1097;
  } else if (CompareUOM(name, "s/l") == 0) {
    myReturn = 1098;
  } else if (CompareUOM(name, "s/m") == 0) {
    myReturn = 123;
  } else if (CompareUOM(name, "s/m3") == 0) {
    myReturn = 124;
  } else if (CompareUOM(name, "sacks") == 0) {
    myReturn = 1101;
  } else if (CompareUOM(name, "scf(60f)") == 0) {
    myReturn = 1102;
  } else if (CompareUOM(name, "sears chain") == 0) {
    myReturn = 311;
  } else if (CompareUOM(name, "sears foot") == 0) {
    myReturn = 491;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "sears link") == 0) {
      myReturn = 788;
    } else if (CompareUOM(name, "sears yard") == 0) {
      myReturn = 1229;
    } else if (CompareUOM(name, "seca") == 0) {
      myReturn = 1109;
    } else if (CompareUOM(name, "second") == 0) {
      myReturn = 121;
    } else if (CompareUOM(name, "second per cubic foot") == 0) {
      myReturn = 1096;
    } else if (CompareUOM(name, "second per litre") == 0) {
      myReturn = 1098;
    } else if (CompareUOM(name, "second per uk quart") == 0) {
      myReturn = 1099;
    } else if (CompareUOM(name, "second per us quart") == 0) {
      myReturn = 1100;
    } else if (CompareUOM(name, "seconds angular") == 0) {
      myReturn = 1109;
    } else if (CompareUOM(name, "seconds/centimetre") == 0) {
      myReturn = 1094;
    } else if (CompareUOM(name, "seconds/cubic metre") == 0) {
      myReturn = 124;
    } else if (CompareUOM(name, "seconds/foot") == 0) {
      myReturn = 1095;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "seconds/inch") == 0) {
      myReturn = 1097;
    } else if (CompareUOM(name, "seconds/metre") == 0) {
      myReturn = 123;
    } else if (CompareUOM(name, "siemens") == 0) {
      myReturn = 120;
    } else if (CompareUOM(name, "siemens/metre") == 0) {
      myReturn = 122;
    } else if (CompareUOM(name, "sievert") == 0) {
      myReturn = 133;
    } else if (CompareUOM(name, "sievert per second") == 0) {
      myReturn = 134;
    } else if (CompareUOM(name, "sieverts per hour") == 0) {
      myReturn = 1123;
    } else if (CompareUOM(name, "sq ft") == 0) {
      myReturn = 457;
    } else if (CompareUOM(name, "sq in") == 0) {
      myReturn = 597;
    } else if (CompareUOM(name, "sq mi") == 0) {
      myReturn = 901;
    } else if (CompareUOM(name, "square centimetre") == 0) {
      myReturn = 319;
    } else if (CompareUOM(name, "square feet") == 0) {
      myReturn = 1112;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "square feet/cubic inch") == 0) {
      myReturn = 459;
    } else if (CompareUOM(name, "square feet/hour") == 0) {
      myReturn = 458;
    } else if (CompareUOM(name, "square feet/second") == 0) {
      myReturn = 460;
    } else if (CompareUOM(name, "square foot") == 0) {
      myReturn = 457;
    } else if (CompareUOM(name, "square inches") == 0) {
      myReturn = 597;
    } else if (CompareUOM(name, "square inches/second") == 0) {
      myReturn = 600;
    } else if (CompareUOM(name, "square inches/square foot") == 0) {
      myReturn = 598;
    } else if (CompareUOM(name, "square inches/square inch") == 0) {
      myReturn = 599;
    } else if (CompareUOM(name, "square kilometres") == 0) {
      myReturn = 681;
    } else if (CompareUOM(name, "square metres") == 0) {
      myReturn = 73;
    } else if (CompareUOM(name, "square metres/cubic centimetre") == 0) {
      myReturn = 806;
    } else if (CompareUOM(name, "square metres/cubic metre") == 0) {
      myReturn = 811;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "square metres/day kilopascal") == 0) {
      myReturn = 807;
    } else if (CompareUOM(name, "square metres/gram") == 0) {
      myReturn = 808;
    } else if (CompareUOM(name, "square metres/hour") == 0) {
      myReturn = 809;
    } else if (CompareUOM(name, "square metres/kilogram") == 0) {
      myReturn = 74;
    } else if (CompareUOM(name, "square metres/mol") == 0) {
      myReturn = 75;
    } else if (CompareUOM(name, "square metres/second") == 0) {
      myReturn = 77;
    } else if (CompareUOM(name, "square metres/second pascal") == 0) {
      myReturn = 76;
    } else if (CompareUOM(name, "square metres/square metre") == 0) {
      myReturn = 810;
    } else if (CompareUOM(name, "square micron metres") == 0) {
      myReturn = 1187;
    } else if (CompareUOM(name, "square microns") == 0) {
      myReturn = 1186;
    } else if (CompareUOM(name, "square miles") == 0) {
      myReturn = 901;
    } else if (CompareUOM(name, "square millimetres") == 0) {
      myReturn = 930;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "square millimetres/second") == 0) {
      myReturn = 932;
    } else if (CompareUOM(name, "square millimetres/square millimetre") == 0) {
      myReturn = 931;
    } else if (CompareUOM(name, "square yards") == 0) {
      myReturn = 1113;
    } else if (CompareUOM(name, "sr") == 0) {
      myReturn = 132;
    } else if (CompareUOM(name, "standard cubic feet/day") == 0) {
      myReturn = 1104;
    } else if (CompareUOM(name, "standard cubic metres at 0 deg celsius") == 0) {
      myReturn = 125;
    } else if (CompareUOM(name, "standard cubic metres at 15 deg celsius") == 0) {
      myReturn = 128;
    } else if (CompareUOM(name, "std cubic feet at 60 deg f") == 0) {
      myReturn = 1102;
    } else if (CompareUOM(name, "std cubic feet at 60 deg f/barrel") == 0) {
      myReturn = 1103;
    } else if (CompareUOM(name, "std cubic feet at 60 deg f/square foot") == 0) {
      myReturn = 1105;
    } else if (CompareUOM(name, "std cubic feet at 60 deg ft/cubic foot") == 0) {
      myReturn = 1106;
    } else if (CompareUOM(name, "std cubic metres / stock tank barrel") == 0) {
      myReturn = 1108;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "std cubic metres at 15 deg c/day") == 0) {
      myReturn = 1107;
    } else if (CompareUOM(name, "std cubic metres, 0 deg c/cubic metre") == 0) {
      myReturn = 127;
    } else if (CompareUOM(name, "std cubic metres, 0 deg c/square metre") == 0) {
      myReturn = 126;
    } else if (CompareUOM(name, "std cubic metres, 15 deg c/cubic metre") == 0) {
      myReturn = 130;
    } else if (CompareUOM(name, "std cubic metres, 15 deg c/second") == 0) {
      myReturn = 131;
    } else if (CompareUOM(name, "std cubic metres, 15 deg c/square metre") == 0) {
      myReturn = 129;
    } else if (CompareUOM(name, "std cubic metres/ 1000 std cubic metre") == 0) {
      myReturn = 1110;
    } else if (CompareUOM(name, "std cubic metres/ std cubic metres") == 0) {
      myReturn = 1111;
    } else if (CompareUOM(name, "steradian") == 0) {
      myReturn = 132;
    } else if (CompareUOM(name, "stock tank barrel at 60 deg f") == 0) {
      myReturn = 1114;
    } else if (CompareUOM(name, "stock tank barrels, 60 deg f/acre") == 0) {
      myReturn = 1115;
    } else if (CompareUOM(name, "stock tank barrels, 60 deg f/barrel") == 0) {
      myReturn = 1116;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "stock tank barrels, 60 deg f/day") == 0) {
      myReturn = 1117;
    } else if (CompareUOM(name, "stock tank barrels/ 1000 std cu ft") == 0) {
      myReturn = 1120;
    } else if (CompareUOM(name, "stock tank barrels/ 1000 std cu metres") == 0) {
      myReturn = 1121;
    } else if (CompareUOM(name, "stock tank barrels/ million std cu ft") == 0) {
      myReturn = 1118;
    } else if (CompareUOM(name, "stock tank barrels/ million std cu mts") == 0) {
      myReturn = 1119;
    } else if (CompareUOM(name, "stock tank barrels/ std cu metres") == 0) {
      myReturn = 1122;
    } else if (CompareUOM(name, "sv") == 0) {
      myReturn = 133;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_t(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "t") == 0) {
    myReturn = 1124;
  } else if (CompareUOM(name, "t/a") == 0) {
    myReturn = 1125;
  } else if (CompareUOM(name, "t/d") == 0) {
    myReturn = 1126;
  } else if (CompareUOM(name, "t/h") == 0) {
    myReturn = 1127;
  } else if (CompareUOM(name, "t/min") == 0) {
    myReturn = 1128;
  } else if (CompareUOM(name, "talbot") == 0) {
    myReturn = 1129;
  } else if (CompareUOM(name, "tbq") == 0) {
    myReturn = 1130;
  } else if (CompareUOM(name, "technical atmosphere") == 0) {
    myReturn = 209;
  } else if (CompareUOM(name, "ten milli second") == 0) {
    myReturn = 333;
  } else if (CompareUOM(name, "ten thousand kilograms per cubic metre") == 0) {
    myReturn = 196;
  } else if (CompareUOM(name, "tenth of an inch") == 0) {
    myReturn = 589;
  } else if (CompareUOM(name, "tera electron volts") == 0) {
    myReturn = 1132;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "terabecquerel") == 0) {
      myReturn = 1130;
    } else if (CompareUOM(name, "terajoules") == 0) {
      myReturn = 1137;
    } else if (CompareUOM(name, "terajoules/year") == 0) {
      myReturn = 1138;
    } else if (CompareUOM(name, "teraohm") == 0) {
      myReturn = 1139;
    } else if (CompareUOM(name, "terawatts") == 0) {
      myReturn = 1166;
    } else if (CompareUOM(name, "terrawatt hours") == 0) {
      myReturn = 1167;
    } else if (CompareUOM(name, "tesla") == 0) {
      myReturn = 135;
    } else if (CompareUOM(name, "tev") == 0) {
      myReturn = 1132;
    } else if (CompareUOM(name, "therm/galuk") == 0) {
      myReturn = 1135;
    } else if (CompareUOM(name, "therm/lbm") == 0) {
      myReturn = 1136;
    } else if (CompareUOM(name, "therms") == 0) {
      myReturn = 1133;
    } else if (CompareUOM(name, "therms/cubic foot") == 0) {
      myReturn = 1134;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "therms/pound mass") == 0) {
      myReturn = 1136;
    } else if (CompareUOM(name, "therms/uk gallon") == 0) {
      myReturn = 1135;
    } else if (CompareUOM(name, "thousand (cubic meter per day)-meter") == 0) {
      myReturn = 194;
    } else if (CompareUOM(name, "thousand barrel feet/day") == 0) {
      myReturn = 850;
    } else if (CompareUOM(name, "thousand barrels") == 0) {
      myReturn = 849;
    } else if (CompareUOM(name, "thousand barrels per day") == 0) {
      myReturn = 618;
    } else if (CompareUOM(name, "thousand barrels/day") == 0) {
      myReturn = 851;
    } else if (CompareUOM(name, "thousand cubic feet") == 0) {
      myReturn = 185;
    } else if (CompareUOM(name, "thousand cubic feet at 60 deg f") == 0) {
      myReturn = 989;
    } else if (CompareUOM(name, "thousand cubic feet per barrel") == 0) {
      myReturn = 186;
    } else if (CompareUOM(name, "thousand cubic feet per day") == 0) {
      myReturn = 187;
    } else if (CompareUOM(name, "thousand cubic feet per day per foot") == 0) {
      myReturn = 188;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "thousand cubic feet per day per psi") == 0) {
      myReturn = 189;
    } else if (CompareUOM(name, "thousand cubic meter per day per meter") == 0) {
      myReturn = 191;
    } else if (CompareUOM(name, "thousand cubic meters per hour per meter") == 0) {
      myReturn = 193;
    } else if (CompareUOM(name, "thousand cubic metres per day") == 0) {
      myReturn = 190;
    } else if (CompareUOM(name, "thousand cubic metres per hour") == 0) {
      myReturn = 192;
    } else if (CompareUOM(name, "thousand feet per hour") == 0) {
      myReturn = 637;
    } else if (CompareUOM(name, "thousand feet per second") == 0) {
      myReturn = 638;
    } else if (CompareUOM(name, "thousand foot pounds force") == 0) {
      myReturn = 636;
    } else if (CompareUOM(name, "thousand kilograms force") == 0) {
      myReturn = 888;
    } else if (CompareUOM(name, "thousand kilograms per cubic metre") == 0) {
      myReturn = 884;
    } else if (CompareUOM(name, "thousand kilograms per inch") == 0) {
      myReturn = 878;
    } else if (CompareUOM(name, "thousand per second") == 0) {
      myReturn = 634;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "thousand pound per square inch, squared") == 0) {
      myReturn = 702;
    } else if (CompareUOM(name, "thousand pounds force") == 0) {
      myReturn = 671;
    } else if (CompareUOM(name, "thousand pounds mass") == 0) {
      myReturn = 672;
    } else if (CompareUOM(name, "thousand pounds mass per inch") == 0) {
      myReturn = 673;
    } else if (CompareUOM(name, "thousand pounds per square inch") == 0) {
      myReturn = 701;
    } else if (CompareUOM(name, "thousand standard cubic feet/day") == 0) {
      myReturn = 990;
    } else if (CompareUOM(name, "thousand std cu ft/ stock tank barrel") == 0) {
      myReturn = 991;
    } else if (CompareUOM(name, "thousand std cubic metres, 15 degc/day") == 0) {
      myReturn = 992;
    } else if (CompareUOM(name, "thousand std cubic metres/ day") == 0) {
      myReturn = 706;
    } else if (CompareUOM(name, "thousand stock tank barrels 60 f") == 0) {
      myReturn = 995;
    } else if (CompareUOM(name, "thousand stock tank barrels,60 deg f/day") == 0) {
      myReturn = 996;
    } else if (CompareUOM(name, "tj") == 0) {
      myReturn = 1137;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "tj/a") == 0) {
      myReturn = 1138;
    } else if (CompareUOM(name, "tohm") == 0) {
      myReturn = 1139;
    } else if (CompareUOM(name, "tonfus.mi") == 0) {
      myReturn = 1148;
    } else if (CompareUOM(name, "tonne") == 0) {
      myReturn = 1124;
    } else if (CompareUOM(name, "tonnes per minute") == 0) {
      myReturn = 1128;
    } else if (CompareUOM(name, "tonnes/day") == 0) {
      myReturn = 1126;
    } else if (CompareUOM(name, "tonnes/hour") == 0) {
      myReturn = 1127;
    } else if (CompareUOM(name, "tonnes/year") == 0) {
      myReturn = 1125;
    } else if (CompareUOM(name, "tons of refrigeration") == 0) {
      myReturn = 1140;
    } else if (CompareUOM(name, "tonuk") == 0) {
      myReturn = 1154;
    } else if (CompareUOM(name, "tonuk/a") == 0) {
      myReturn = 1155;
    } else if (CompareUOM(name, "tonuk/d") == 0) {
      myReturn = 1156;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "tonuk/h") == 0) {
      myReturn = 1157;
    } else if (CompareUOM(name, "tonuk/min") == 0) {
      myReturn = 1158;
    } else if (CompareUOM(name, "tonus") == 0) {
      myReturn = 1159;
    } else if (CompareUOM(name, "tonus/a") == 0) {
      myReturn = 1160;
    } else if (CompareUOM(name, "tonus/d") == 0) {
      myReturn = 1161;
    } else if (CompareUOM(name, "tonus/ft2") == 0) {
      myReturn = 1162;
    } else if (CompareUOM(name, "tonus/h") == 0) {
      myReturn = 1163;
    } else if (CompareUOM(name, "tonus/min") == 0) {
      myReturn = 1164;
    } else if (CompareUOM(name, "torr") == 0) {
      myReturn = 1165;
    } else if (CompareUOM(name, "trillion cubic feet") == 0) {
      myReturn = 1131;
    } else if (CompareUOM(name, "troy ounces") == 0) {
      myReturn = 1035;
    } else if (CompareUOM(name, "tw") == 0) {
      myReturn = 1166;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "tw.h") == 0) {
      myReturn = 1167;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_u(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "u.s. survey mile") == 0) {
    myReturn = 909;
  } else if (CompareUOM(name, "u.s. survey square mile") == 0) {
    myReturn = 910;
  } else if (CompareUOM(name, "ua") == 0) {
    myReturn = 1168;
  } else if (CompareUOM(name, "ua/cm2") == 0) {
    myReturn = 1169;
  } else if (CompareUOM(name, "ua/in2") == 0) {
    myReturn = 1170;
  } else if (CompareUOM(name, "ubar") == 0) {
    myReturn = 1171;
  } else if (CompareUOM(name, "uc") == 0) {
    myReturn = 1172;
  } else if (CompareUOM(name, "uci") == 0) {
    myReturn = 1174;
  } else if (CompareUOM(name, "ueuc") == 0) {
    myReturn = 1219;
  } else if (CompareUOM(name, "uf") == 0) {
    myReturn = 1176;
  } else if (CompareUOM(name, "uf/m") == 0) {
    myReturn = 1177;
  } else if (CompareUOM(name, "ug") == 0) {
    myReturn = 1178;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "ug/cm3") == 0) {
      myReturn = 1179;
    } else if (CompareUOM(name, "uh") == 0) {
      myReturn = 1180;
    } else if (CompareUOM(name, "uh/m") == 0) {
      myReturn = 1181;
    } else if (CompareUOM(name, "uhz") == 0) {
      myReturn = 1182;
    } else if (CompareUOM(name, "uj") == 0) {
      myReturn = 1183;
    } else if (CompareUOM(name, "uk fluid ounce") == 0) {
      myReturn = 427;
    } else if (CompareUOM(name, "uk gallon") == 0) {
      myReturn = 507;
    } else if (CompareUOM(name, "uk gallons per day") == 0) {
      myReturn = 508;
    } else if (CompareUOM(name, "uk gallons per thousand uk gallons") == 0) {
      myReturn = 516;
    } else if (CompareUOM(name, "uk gallons/1000 barrels") == 0) {
      myReturn = 518;
    } else if (CompareUOM(name, "uk gallons/cubic foot") == 0) {
      myReturn = 509;
    } else if (CompareUOM(name, "uk gallons/hour") == 0) {
      myReturn = 510;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "uk gallons/hour foot") == 0) {
      myReturn = 511;
    } else if (CompareUOM(name, "uk gallons/hour inch") == 0) {
      myReturn = 513;
    } else if (CompareUOM(name, "uk gallons/hour square foot") == 0) {
      myReturn = 512;
    } else if (CompareUOM(name, "uk gallons/hour square inch") == 0) {
      myReturn = 514;
    } else if (CompareUOM(name, "uk gallons/hour/hour") == 0) {
      myReturn = 515;
    } else if (CompareUOM(name, "uk gallons/mile") == 0) {
      myReturn = 519;
    } else if (CompareUOM(name, "uk gallons/minute") == 0) {
      myReturn = 520;
    } else if (CompareUOM(name, "uk gallons/minute foot") == 0) {
      myReturn = 521;
    } else if (CompareUOM(name, "uk gallons/minute square foot") == 0) {
      myReturn = 522;
    } else if (CompareUOM(name, "uk gallons/minute/minute") == 0) {
      myReturn = 523;
    } else if (CompareUOM(name, "uk gallons/pound mass") == 0) {
      myReturn = 517;
    } else if (CompareUOM(name, "uk hundredweight") == 0) {
      myReturn = 343;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "uk pint") == 0) {
      myReturn = 1077;
    } else if (CompareUOM(name, "uk pints/1000 barrels") == 0) {
      myReturn = 1079;
    } else if (CompareUOM(name, "uk pints/horsepower hour") == 0) {
      myReturn = 1078;
    } else if (CompareUOM(name, "uk quarts") == 0) {
      myReturn = 1082;
    } else if (CompareUOM(name, "uk ton feet squared") == 0) {
      myReturn = 1142;
    } else if (CompareUOM(name, "uk tons") == 0) {
      myReturn = 1154;
    } else if (CompareUOM(name, "uk tons force") == 0) {
      myReturn = 1141;
    } else if (CompareUOM(name, "uk tons force/foot") == 0) {
      myReturn = 1143;
    } else if (CompareUOM(name, "uk tons force/square foot") == 0) {
      myReturn = 1144;
    } else if (CompareUOM(name, "uk tons/day") == 0) {
      myReturn = 1156;
    } else if (CompareUOM(name, "uk tons/hour") == 0) {
      myReturn = 1157;
    } else if (CompareUOM(name, "uk tons/minute") == 0) {
      myReturn = 1158;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "uk tons/year") == 0) {
      myReturn = 1155;
    } else if (CompareUOM(name, "um") == 0) {
      myReturn = 1184;
    } else if (CompareUOM(name, "um/s") == 0) {
      myReturn = 1185;
    } else if (CompareUOM(name, "um2") == 0) {
      myReturn = 1186;
    } else if (CompareUOM(name, "um2.m") == 0) {
      myReturn = 1187;
    } else if (CompareUOM(name, "umol") == 0) {
      myReturn = 1189;
    } else if (CompareUOM(name, "un") == 0) {
      myReturn = 1190;
    } else if (CompareUOM(name, "unitless") == 0) {
      myReturn = 1191;
    } else if (CompareUOM(name, "uohm") == 0) {
      myReturn = 1192;
    } else if (CompareUOM(name, "uohm/ft") == 0) {
      myReturn = 1193;
    } else if (CompareUOM(name, "uohm/m") == 0) {
      myReturn = 1194;
    } else if (CompareUOM(name, "upa") == 0) {
      myReturn = 1195;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "upsi") == 0) {
      myReturn = 1196;
    } else if (CompareUOM(name, "urad") == 0) {
      myReturn = 1197;
    } else if (CompareUOM(name, "us") == 0) {
      myReturn = 1199;
    } else if (CompareUOM(name, "us fluid ounces") == 0) {
      myReturn = 428;
    } else if (CompareUOM(name, "us gallons") == 0) {
      myReturn = 524;
    } else if (CompareUOM(name, "us gallons per day") == 0) {
      myReturn = 527;
    } else if (CompareUOM(name, "us gallons per ten barrels") == 0) {
      myReturn = 525;
    } else if (CompareUOM(name, "us gallons per thousand us gallons") == 0) {
      myReturn = 536;
    } else if (CompareUOM(name, "us gallons/1000 barrels") == 0) {
      myReturn = 538;
    } else if (CompareUOM(name, "us gallons/94 lb sack") == 0) {
      myReturn = 506;
    } else if (CompareUOM(name, "us gallons/barrels") == 0) {
      myReturn = 526;
    } else if (CompareUOM(name, "us gallons/cubic foot") == 0) {
      myReturn = 529;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "us gallons/foot") == 0) {
      myReturn = 528;
    } else if (CompareUOM(name, "us gallons/foot hour") == 0) {
      myReturn = 531;
    } else if (CompareUOM(name, "us gallons/hour") == 0) {
      myReturn = 530;
    } else if (CompareUOM(name, "us gallons/hour inch") == 0) {
      myReturn = 533;
    } else if (CompareUOM(name, "us gallons/hour square foot") == 0) {
      myReturn = 532;
    } else if (CompareUOM(name, "us gallons/hour square inch") == 0) {
      myReturn = 534;
    } else if (CompareUOM(name, "us gallons/hour/hour") == 0) {
      myReturn = 535;
    } else if (CompareUOM(name, "us gallons/mile") == 0) {
      myReturn = 539;
    } else if (CompareUOM(name, "us gallons/minute") == 0) {
      myReturn = 540;
    } else if (CompareUOM(name, "us gallons/minute foot") == 0) {
      myReturn = 541;
    } else if (CompareUOM(name, "us gallons/minute square foot") == 0) {
      myReturn = 542;
    } else if (CompareUOM(name, "us gallons/minute/minute") == 0) {
      myReturn = 543;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "us gallons/pound mass") == 0) {
      myReturn = 537;
    } else if (CompareUOM(name, "us gallons/uk ton") == 0) {
      myReturn = 545;
    } else if (CompareUOM(name, "us gallons/us ton") == 0) {
      myReturn = 546;
    } else if (CompareUOM(name, "us gals/1000 std cubic feet, 60 deg f") == 0) {
      myReturn = 544;
    } else if (CompareUOM(name, "us hundredweight") == 0) {
      myReturn = 344;
    } else if (CompareUOM(name, "us pint per ten barrel") == 0) {
      myReturn = 1081;
    } else if (CompareUOM(name, "us pints") == 0) {
      myReturn = 1080;
    } else if (CompareUOM(name, "us quarts") == 0) {
      myReturn = 1083;
    } else if (CompareUOM(name, "us survey chain") == 0) {
      myReturn = 313;
    } else if (CompareUOM(name, "us survey foot") == 0) {
      myReturn = 492;
    } else if (CompareUOM(name, "us survey inch") == 0) {
      myReturn = 608;
    } else if (CompareUOM(name, "us survey link") == 0) {
      myReturn = 789;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "us ton force miles/barrel") == 0) {
      myReturn = 1149;
    } else if (CompareUOM(name, "us tons") == 0) {
      myReturn = 1159;
    } else if (CompareUOM(name, "us tons force") == 0) {
      myReturn = 1145;
    } else if (CompareUOM(name, "us tons force feet") == 0) {
      myReturn = 1146;
    } else if (CompareUOM(name, "us tons force feet squared") == 0) {
      myReturn = 1147;
    } else if (CompareUOM(name, "us tons force miles") == 0) {
      myReturn = 1148;
    } else if (CompareUOM(name, "us tons force miles/foot") == 0) {
      myReturn = 1150;
    } else if (CompareUOM(name, "us tons force/foot") == 0) {
      myReturn = 1151;
    } else if (CompareUOM(name, "us tons force/square foot") == 0) {
      myReturn = 1152;
    } else if (CompareUOM(name, "us tons force/square inch") == 0) {
      myReturn = 1153;
    } else if (CompareUOM(name, "us tons/day") == 0) {
      myReturn = 1161;
    } else if (CompareUOM(name, "us tons/hour") == 0) {
      myReturn = 1163;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "us tons/minute") == 0) {
      myReturn = 1164;
    } else if (CompareUOM(name, "us tons/square foot") == 0) {
      myReturn = 1162;
    } else if (CompareUOM(name, "us tons/year") == 0) {
      myReturn = 1160;
    } else if (CompareUOM(name, "us/ft") == 0) {
      myReturn = 1200;
    } else if (CompareUOM(name, "us/m") == 0) {
      myReturn = 1201;
    } else if (CompareUOM(name, "ut") == 0) {
      myReturn = 1202;
    } else if (CompareUOM(name, "uv") == 0) {
      myReturn = 1203;
    } else if (CompareUOM(name, "uv/ft") == 0) {
      myReturn = 1204;
    } else if (CompareUOM(name, "uv/m") == 0) {
      myReturn = 1205;
    } else if (CompareUOM(name, "uw") == 0) {
      myReturn = 1206;
    } else if (CompareUOM(name, "uw/m3") == 0) {
      myReturn = 1207;
    } else if (CompareUOM(name, "uwb") == 0) {
      myReturn = 1208;
    }
  }
  return myReturn;
}

RESCUEINT32 RescueUOM::IndexFromName_w(const RESCUECHAR *name) {
  RESCUEINT32 myReturn = -1;
  if (CompareUOM(name, "w") == 0) {
    myReturn = 139;
  } else if (CompareUOM(name, "w/cm2") == 0) {
    myReturn = 1212;
  } else if (CompareUOM(name, "w/k") == 0) {
    myReturn = 140;
  } else if (CompareUOM(name, "w/kw") == 0) {
    myReturn = 1213;
  } else if (CompareUOM(name, "w/m.k") == 0) {
    myReturn = 141;
  } else if (CompareUOM(name, "w/m2") == 0) {
    myReturn = 142;
  } else if (CompareUOM(name, "w/m2.k") == 0) {
    myReturn = 143;
  } else if (CompareUOM(name, "w/m2.sr") == 0) {
    myReturn = 144;
  } else if (CompareUOM(name, "w/m3") == 0) {
    myReturn = 145;
  } else if (CompareUOM(name, "w/m3.k") == 0) {
    myReturn = 146;
  } else if (CompareUOM(name, "w/mm2") == 0) {
    myReturn = 1214;
  } else if (CompareUOM(name, "w/sr") == 0) {
    myReturn = 147;
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "w/w") == 0) {
      myReturn = 1215;
    } else if (CompareUOM(name, "watt") == 0) {
      myReturn = 139;
    } else if (CompareUOM(name, "watts per delta kelvin") == 0) {
      myReturn = 140;
    } else if (CompareUOM(name, "watts per square millimeter") == 0) {
      myReturn = 1214;
    } else if (CompareUOM(name, "watts/cubic metre") == 0) {
      myReturn = 145;
    } else if (CompareUOM(name, "watts/cubic metre kelvin") == 0) {
      myReturn = 146;
    } else if (CompareUOM(name, "watts/kilowatt") == 0) {
      myReturn = 1213;
    } else if (CompareUOM(name, "watts/metre kelvin") == 0) {
      myReturn = 141;
    } else if (CompareUOM(name, "watts/square centimetre") == 0) {
      myReturn = 1212;
    } else if (CompareUOM(name, "watts/square metre") == 0) {
      myReturn = 142;
    } else if (CompareUOM(name, "watts/square metre kelvin") == 0) {
      myReturn = 143;
    } else if (CompareUOM(name, "watts/square metre steradian") == 0) {
      myReturn = 144;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "watts/steradian") == 0) {
      myReturn = 147;
    } else if (CompareUOM(name, "watts/watt") == 0) {
      myReturn = 1215;
    } else if (CompareUOM(name, "wb") == 0) {
      myReturn = 148;
    } else if (CompareUOM(name, "wb.m") == 0) {
      myReturn = 149;
    } else if (CompareUOM(name, "wb/m") == 0) {
      myReturn = 150;
    } else if (CompareUOM(name, "wb/mm") == 0) {
      myReturn = 1216;
    } else if (CompareUOM(name, "weber") == 0) {
      myReturn = 148;
    } else if (CompareUOM(name, "weber metres") == 0) {
      myReturn = 149;
    } else if (CompareUOM(name, "webers/metre") == 0) {
      myReturn = 150;
    } else if (CompareUOM(name, "webers/millimetre") == 0) {
      myReturn = 1216;
    } else if (CompareUOM(name, "weeks") == 0) {
      myReturn = 1217;
    } else if (CompareUOM(name, "weight parts per million") == 0) {
      myReturn = 1219;
    }
  }
  if (myReturn == -1) {
    if (CompareUOM(name, "weight percent") == 0) {
      myReturn = 1218;
    }
  }
  return myReturn;
}
