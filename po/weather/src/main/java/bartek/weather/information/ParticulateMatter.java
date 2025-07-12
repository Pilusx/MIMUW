package bartek.weather.information;

public class ParticulateMatter {
	private double pM25;
	private double pM10;
	private boolean isKnownPM25;
	private boolean isKnownPM10;
	
	public ParticulateMatter() {
		super();
		isKnownPM25 = false;
		isKnownPM10 = false;
	}
	
	public void setPM25(double pM25) {
		this.pM25 = pM25;
		this.isKnownPM25 = true;
	}

	public void setPM10(double pM10) {
		this.pM10 = pM10;
		this.isKnownPM10 = true;
	}
	
	
	public double getPM25() throws Exception {
		if(isKnownPM25)
			return pM25;
		else
			throw new Exception();
	}
	
	public double getPM10() throws Exception {
		if(isKnownPM10)
			return pM10;
		else
			throw new Exception();
	}
}
