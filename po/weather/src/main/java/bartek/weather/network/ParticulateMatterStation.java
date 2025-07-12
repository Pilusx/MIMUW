package bartek.weather.network;

import bartek.weather.information.ParticulateMatter;
import bartek.weather.information.Place;

public interface ParticulateMatterStation {
	public ParticulateMatter getParticulateMatter(Place place);
}
