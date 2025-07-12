package bartek.weather.network;

import java.io.InputStream;
import java.net.URL;

import org.json.JSONObject;
import org.json.JSONTokener;

import bartek.weather.information.Air;
import bartek.weather.information.Place;
import bartek.weather.information.Wind;

public class OpenWeather implements AirStation {
	private static OpenWeather instance = null;
	
	public static OpenWeather getInstance(){
		if(instance == null){
			instance = new OpenWeather();
		}
		return instance;
	}
	
	public Air getAir(Place place) {
		URL url;
		try {
			url = new URL(place.getOpenWeatherLink());
			InputStream is = url.openStream();
		
			JSONTokener jt = new JSONTokener(is);
			JSONObject jo = new JSONObject(jt);
			
			return openWeatherToAir(jo);
		} catch (Exception e){
			return new Air();
		}
	}
	
	private static Air openWeatherToAir(JSONObject jo){
		JSONObject jomain = jo.getJSONObject("main");
		JSONObject jowind = jo.getJSONObject("wind");
		JSONObject joclouds = jo.getJSONObject("clouds");
		
		double pressure = jomain.getDouble("pressure");
		double cloudiness = joclouds.getDouble("all");
		double humidity = jomain.getDouble("humidity");
		double temperature = kelvinToCelsius(jomain.getDouble("temp"));
		double windAngle = jowind.getDouble("deg");
		double windSpeed = jowind.getDouble("speed");
		
		Wind wind = new Wind();
		wind.setSpeed(windSpeed);
		wind.setAngle(windAngle);
		
		Air air = new Air();
		air.setPressure(pressure);
		air.setCloudiness(cloudiness);
		air.setHumidity(humidity);
		air.setTemperature(temperature);
		air.setWind(wind);
		
		System.out.println("(OpenWeather) [Downloaded 'Air']");
		
		return air;
	}
	
	private static double kelvinToCelsius(double c){
		return c-273.15;
	}
}
