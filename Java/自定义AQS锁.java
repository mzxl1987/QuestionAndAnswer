import java.util.concurrent.locks.AbstractQueuedSynchronizer;

import org.w3c.dom.views.AbstractView;

public class BooleanLock{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	static final class Sync extends AbstractQueuedSynchronizer{
		public void lock(){ super.acquire(1); }
		public void unlock(){ super.release(1); }
		public boolean tryLock(){ return super.tryAcquire(1); }
		public boolean tryUnlock(){ return super.tryRelease(1); }
		public boolean isLock(){ return super.isHeldExclusively(); }
		
		protected final boolean tryAcquire(int acquires){
			final Thread thread = Thread.currentThread();
			int c = getState();
			if(!hasQueuedPredecessors() &&
			   compareAndSetState(0, acquires)){
				setExclusiveOwnerThread(thread);
				return true;
			}else if(thread == getExclusiveOwnerThread()){
				int nextC = c + acquires;
				if(nextC < 0) throw new Error(" MAX lock count exceeded ");
				setState(nextC);
				return true;
			}
			return false;
		}
		
		protected final boolean tryRelease(int releases){
			int c = getState() - releases;
			if(Thread.currentThread() != getExclusiveOwnerThread())
				throw new IllegalMonitorStateException();
			boolean free = false;
			if(c == 0){
				free = true;
				setExclusiveOwnerThread(null);
			}
			setState(c);
			return free;
		}
		
	}
	
	private Sync sync;
	
	public BooleanLock(){
		sync = new Sync();
	}
	
	public void lock(){ sync.lock(); }
	public void unlock(){ sync.unlock(); }
	public boolean tryLock(){ return sync.tryLock(); }
	public boolean tryUnlock(){ return sync.tryUnlock(); }
	public boolean isLock(){ return sync.isLock(); }
	
}
