package bartek.weather.network;

import bartek.weather.information.Air;
import bartek.weather.information.Place;

public interface AirStation {
	public Air getAir(Place place);
}
