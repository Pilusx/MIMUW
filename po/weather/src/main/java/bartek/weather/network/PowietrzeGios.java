package bartek.weather.network;

import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;

import org.json.*;

import bartek.weather.information.*;

public class PowietrzeGios implements ParticulateMatterStation {
	private static PowietrzeGios instance = null;
	private static String link = "http://powietrze.gios.gov.pl/pjp/current/getAQIDetailsList?param=AQI";
	
	public static PowietrzeGios getInstance(){
		if(instance == null){
			instance = new PowietrzeGios();
		}
		return instance;
	}
	
	
	public ParticulateMatter getParticulateMatter(Place place){
		URL url;
		ParticulateMatter pm = new ParticulateMatter();
		try {
			url = new URL(link);
		} catch (MalformedURLException e) {
			return pm;
		}
		
		try(InputStream is = url.openStream()) {
			JSONTokener jt = new JSONTokener(is);
			JSONArray ja = new JSONArray(jt);
			
			for(int i = 0; i < ja.length(); i++){
				JSONObject jo = ja.getJSONObject(i);
				String s = jo.getString("stationName");
				if(s.contains(place.getPowietrzeGiosCityName())){
					JSONObject values = jo.getJSONObject("values");
					double pm25 = values.getInt("PM2.5");
					double pm10 = values.getInt("PM10");
					
					pm.setPM10(pm10);
					pm.setPM25(pm25);
					
					System.out.println("(PowietrzeGios) [Downloaded 'Particulate Matter' from: " + s + "]");
					return pm;	
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return pm;
	}	
	
}
