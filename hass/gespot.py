#
# Save to config/custom_components/sensor/
# 
# Usage:
# 
# sensor: 
#   - platform: gespot
#

from homeassistant.helpers.entity import Entity

import logging
import requests
from datetime import timedelta


SCAN_INTERVAL = timedelta(hours=1)
_LOGGER = logging.getLogger(__name__)



def setup_platform(hass, config, add_devices, discovery_info=None):
    """Setup the sensor platform."""
    add_devices([GESensor()])


class GESensor(Entity):
    """Representation of a Sensor."""

    def __init__(self):
        """Initialize the sensor."""
        self.update()

    @property
    def name(self):
        """Return the name of the sensor."""
        return 'ge_spot'

    @property
    def unit_of_measurement(self):
        return 'øre'

    @property
    def state(self):
        """Return the state of the sensor."""
        return self._state
    
    def update(self):
        url = "https://www.ge.no/api/price/area/NO1"
        resp = requests.get(url=url)
        data = resp.json()
        #print(data) 

        for product in data:
            #print(product['product_id'])
            if(product['product_id'] == 7):
                print(product['name'], product['price'])
                self._state=product['price']

        _LOGGER.info("GE prices updated: ", self._state)

# testing
#ge = GESensor()
#ge.update()
