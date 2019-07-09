```
import java.io.Serializable;
import java.lang.reflect.Field;

import sun.misc.Unsafe;

public class AtomicV implements Serializable {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	
	private static final Unsafe unsafe;
	private static final Field getUnsafe;
	private static final long vOffset; 
	
	static {
		
		try{
			getUnsafe = Unsafe.class.getDeclaredField("theUnsafe");
	        getUnsafe.setAccessible(true);
	        unsafe = (Unsafe) getUnsafe.get(null);
			vOffset = unsafe.objectFieldOffset(AtomicV.class.getDeclaredField("value"));
		}catch(Exception e){throw new Error(e);}
		
	}
	
	private volatile int value;
	
	public AtomicV(){ }
	
	public final int getAndPlus(int a){
		return unsafe.getAndAddInt(this, vOffset, a);
	}
	
	public final int plusAndGet(int a){
		return unsafe.getAndAddInt(this, vOffset, a) + a;
	}
	
	public final int getAndMinus(int a){
		return unsafe.getAndAddInt(this, vOffset, -a);
	}
	
	public final int minusAndGet(int a){
		return unsafe.getAndAddInt(this, vOffset, -a) - a;
	}
	
	
}

```
