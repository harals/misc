#
# Usage:
# 
# sensor: 
#   - platform: gespot
#

from homeassistant.helpers.entity import Entity
from homeassistant.helpers.event import track_time_interval
from homeassistant.const import (EVENT_HOMEASSISTANT_STOP)
from homeassistant.util import Throttle
import homeassistant.util.dt as dt_util


import time
import logging
import requests
from datetime import timedelta

SCAN_INTERVAL = timedelta(hours=1)
MIN_TIME_BETWEEN_UPDATES = timedelta(hours=1)


_LOGGER = logging.getLogger(__name__)



def setup_platform(hass, config, add_entities, discovery_info=None):
    """Setup the sensor platform."""
    ges = GESensor(hass)

    hass.data['gespot'] = ges

    def ges_refresh(event_time):
        hass.data['gespot'].update()
        
    track_time_interval(hass, ges_refresh, SCAN_INTERVAL)
    add_entities([ges])
    return True



class GESensor(Entity):
    """Representation of a Sensor."""

    def __init__(self, hass):
        """Initialize the sensor."""
        self._state = None
        self.update()

    @property
    def name(self):
        """Return the name of the sensor."""
        return 'ge_spot'

    @property
    def unit_of_measurement(self):
        return 'øre/kWh'

    @property
    def state(self):
        """Return the state of the sensor."""
        return self._state

    @Throttle(MIN_TIME_BETWEEN_UPDATES)
    def update(self):
        url = "https://www.ge.no/api/price/area/NO1"
        resp = requests.get(url=url)
        data = resp.json()

        for product in data:
            if(product['product_id'] == 7):
                print('GE spotpris update')
                self._state=product['price']

