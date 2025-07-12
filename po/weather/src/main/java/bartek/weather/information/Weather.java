package bartek.weather.information;

import java.time.LocalTime;
import bartek.weather.network.*;


public class Weather {
	private LocalTime time;
	private Place place;
	private Air air; // returns null if failed loading
	private ParticulateMatter particulateMatter;
	
	public Weather(WeatherSource weatherSource, Place place){
		time = LocalTime.now();
		this.place = place;
		
		loadParticulateMatter();
		loadAir(weatherSource);
	}
	
	private void loadAir(WeatherSource weatherSource) {
		try {
			if(weatherSource == WeatherSource.Meteo){
				air = Meteo.getInstance().getAir(place);
			}
			else if (weatherSource == WeatherSource.OpenWeather){
				air = OpenWeather.getInstance().getAir(place);
			}
			else
				air = null;
		} catch (Exception e){
			air = null;
			System.err.println(e + "\nFailed loading Air");
		}
	}

	private void loadParticulateMatter() {
		try {
			particulateMatter = PowietrzeGios.getInstance().getParticulateMatter(place);
		} catch(Exception e){
			particulateMatter = null;
			System.err.println(e + "\nFailed loading ParticulateMatter");
			e.printStackTrace(System.err);
		}
	}
	
	public LocalTime getTime(){
		return time;
	}	
	
	public Place getPlace(){
		return place;
	}
	
	public Air getAir() {
		return air;
	}
	public ParticulateMatter getParticulateMatter() {
		return particulateMatter;
	}
	
}
