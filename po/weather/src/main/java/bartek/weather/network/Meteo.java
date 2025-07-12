package bartek.weather.network;

import java.io.IOException;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.select.Elements;

import bartek.weather.information.Air;
import bartek.weather.information.Place;
import bartek.weather.information.Wind;

public class Meteo implements AirStation {
	private static Meteo instance = null;
	
	private static String temperatureParam = "TA";
	private static String pressureParam = "PR";
	private static String humidityParam = "RH";
	private static String windAngleParam = "WD";
	private static String windSpeedParam = "WV";
	
	public static Meteo getInstance(){
		if(instance == null){
			instance = new Meteo();
		}
		return instance;
	}
	
	
	private static double extractParamElement(String param, Elements elements){
		String value = elements.select("#PARAM_" + param).first().html();
		value = value.replace(',', '.');
		return Double.parseDouble(value);
	}
	
	public Air getAir(Place place){
		Document doc;
		try {
			doc = Jsoup.connect(place.getMeteoLink()).get();
		} catch (IOException e){
			return new Air();
		}
		
		Elements strongs = doc.body().children().select("strong");
		
		double pressure = extractParamElement(pressureParam, strongs);
		double humidity = extractParamElement(humidityParam, strongs);
		double temperature = extractParamElement(temperatureParam, strongs);
		
		double windAngle = extractParamElement(windAngleParam, strongs);
		double windSpeed = extractParamElement(windSpeedParam, strongs);
		
		Wind wind = new Wind();
		wind.setAngle(windAngle);
		wind.setSpeed(windSpeed);
		
		Air air = new Air();
		air.setPressure(pressure);
		air.setHumidity(humidity);
		air.setTemperature(temperature);
		air.setWind(wind);
		
		System.out.println("(Meteo) [Downloaded 'Air']");
		return air;
	}
	
}
