 * 4.并行程序开发及优化
 * 4.1.并行程序设计模式
 * [4.1.1 Future 模式](#411Future模式)
 * [4.1.2.MasterWorker模式](#412MasterWorker模式)
 * [4.1.3.保护暂停模式](#413保护暂停模式)
 * [4.1.4.不变模式](#414不变模式)
 * [4.1.5.生产者消费者模式](#415生产者消费者模式)	




------


#### 4.1.1.Future模式
> 简单的说:Future模式就是将原有的比较耗时的业务由[同步]方式调用变成了[异步]; 不必等到数据处理完全再去执行其他的业务; 而是先返回假数据, 待业务处理完全在回调;
下面通过代码一一说明:
###### Main.java
>模拟数据的请求 & 业务执行流程
```java
public static void main(String[] args){
  Client client = new Client();
  //此处立即返回结果,得到FutureData,而不是RealData
  Data data = client.request("name");
  System.out.println("请求完毕");
  try{
    //使用Sleep代替其他的业务处理
    Thread.sleep(2000);
  }catch(Exception e){}
  //使用真实的数据
  System.out.println("数据 = " + data.getResult());
}
```
###### Client.java
> 模拟异步的实现原理,有点像[借腹生蛋],用FutureData的壳,生RealData的蛋
```
public class Client(){
  public Data request(final String queryStr){
    final FutureData future = new FutureData();
    new Thread(){
      public void run(){
        RealData realdata = new RealData(queryStr);
        future.setRealData(realData);
      }
    }.start();
    return future;
  }
}
```
###### Data.java
```
public interface Data {
  public String getResult();
}
```
###### FutureData.java
> FutureData如何实现[借腹生蛋]的,通过notifyAll(),wait()函数实现异步回调的功能
```
public class FutureData implements Data {
  protected RealData realdata = null;
  protected boolean isReady = false;
  public synchronized void setRealData(RealData realdata){
    if(isReady){
      return;
    }
    this.readdata = readdata;
    isReady = true;
    notifyAll();      //通知getResult
  }
  
  public synchronized String getRealData(){
    while(!isReady){
      try{
        wait();
      }catch(Exception e){}
    }
    return this.realdata.result;
  }
}
```
###### RealData.java
```
public class RealData implements Data {
  protected final String result;
  public RealData(String para){
    //使用sleep模式耗时业务
    StringBuffer sb = new StringBuffer();
    for(int i = 0; i < 10; i++){  
      sb.append(para);
      try{
        Thread.sleep(100);
      }catch(Exception e){}
    }
    result = sb.toString();
  }
  
  public String getResult(){
    return this.result;
  }
}
```
上述是自己实现的Future模式, 在JDK中也有Future模式,可以实现interface Callable接口来实现future模式;代码如下:
```
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.FutureTask;

public class Test {

	public static void main(String[] args) throws InterruptedException, ExecutionException {
		
		FutureTask<String> ft = new FutureTask<>(new RealData("Hello"));  //创建任务
		ExecutorService es = Executors.newFixedThreadPool(1);             //创建线程池
		es.submit(ft);                                                    //执行任务
		System.out.println("调用完毕!");
		try {
			Thread.sleep(1000);   //模拟其他业务
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
		System.out.println("数据 = " + ft.get());
	}
	
	
	
}

class RealData implements Callable<String> {

	private String para;
	
  //这么做可以更快的完成RealData的构造
	public RealData(String para) {
		this.para = para;
	}
	
  //重写call函数
	@Override
	public String call() throws Exception {
		
		StringBuffer sb = new StringBuffer();
		
		sb.append(para);
		Thread.sleep(10000);   //使用sleep模拟耗时的业务
		
		return sb.toString();
	}
	
}
```

#### 4.1.2.MasterWorker模式
> 核心思想:系统由两类进程工作:Master进程&Worker进程.Master进程主要负责接收和分配任务,Worker进程主要执行任务,并将执行结果返回;Master负责归纳和汇总，从而的到最终的结果;对于用户来说，用提交了任务,Master分配完任务之后立即返回,并不会在执行的过程中等待,处理的过程是异步的;
`下面利用求1~100的立方和,简单的代码实现Master-Worker框架
```
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;
import java.util.Queue;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutionException;

public class Test {

	public static void main(String[] args) throws InterruptedException, ExecutionException {
		
		//计算1~100的立方和
		Master m = new Master(new PlusWorker(), 5);
		for(int i = 1; i <= 100; i++) {
			m.submit(i);
		}
		m.execute();
		
		//不停的计算和汇总
		long result = 0;
		Map<String,Object> resultMap = m.getResultMap();
		//判断是否执行完 & 是否所有的结果都被取出
		while(resultMap.size() > 0 || !m.isComplete()) {
			Set<String> keys = resultMap.keySet();
			String key = null;
			if(!keys.isEmpty()) {
				key = keys.stream().findFirst().get();  //获取一个key
			}
			
			Integer i = null;
			if(key != null) i = (Integer)resultMap.get(key);  //获取结果
			if(i != null) result += i;                        //求和
			if(key != null) resultMap.remove(key);            //删除已经计算过的结果
			
		}
		
		System.out.println(result);
		
	}
	
	
	
}

class Worker implements Runnable{

	protected Queue<Object> workQueue;
	protected Map<String,Object> resultMap;
	
	 
	
	public void setWorkQueue(Queue<Object> workQueue) {
		this.workQueue = workQueue;
	}



	public void setResultMap(Map<String, Object> resultMap) {
		this.resultMap = resultMap;
	}

	public Object handle(Object input) {
		return input;
	}

	@Override
	public void run() {
		while(true) {
			Object b = workQueue.poll();  //取出任务
			if(b == null) return;         //所有任务完成则返回
			Object r = handle(b);         //执行任务,并返回结果
			resultMap.put(Integer.toString(r.hashCode()), r);  //将结果放置结果队列
		}
	}
	
}

class PlusWorker extends Worker{
	@Override
	public Object handle(Object input) {
		Integer i = (Integer)input;
		System.out.println("计算立方:" + input);
		return i*i*i;
	}
}

class Master{
	//任务队列
	protected Queue<Object> workQueue = new ConcurrentLinkedQueue<Object>();
	//Worker队列
	protected Map<String,Thread> threadMap = new HashMap<String,Thread>();
	//Worker处理的结果集
	protected Map<String,Object> resultMap = new ConcurrentHashMap<String,Object>();
	
	//初始化工作线程
	public Master(Worker worker,int countWorker) {
		worker.setResultMap(resultMap);
		worker.setWorkQueue(workQueue);
		
		for(int i = 0; i < countWorker; i++) {
			threadMap.put(Integer.toString(i), new Thread(worker,Integer.toString(i)));
		}
	}
	
	//判断是否完成
	public boolean isComplete() {
		for (Thread t : threadMap.values()) {
			if(t.getState() != Thread.State.TERMINATED) return false;
		}
		return true;
	}
	
	//提交任务
	public void submit(Object obj) {
		workQueue.add(obj);
	}
	
	//启动worker线程
	public void execute() {
		threadMap.values().stream().forEach((t)->{
				t.start();
		});
		
	}

	public Map<String, Object> getResultMap() {
		return resultMap;
	}
	
}
```
#### 4.1.3.保护暂停模式
* 核心思想: 当服务进程准备好时,才提供服务
* 使用场景: 服务器短时间承受大量的客户端的请求,客户端的数量可能超过了服务器的负载能力,而服务器又不能丢弃客户端的请求。此时最佳的处理方案是让客户端进入请求队列,由服务器一个接一个的处理.这样避免了客户端请求的丢失,也能够缓解服务器同时处理太多导致崩溃。
```
import java.util.LinkedList;
import java.util.concurrent.ExecutionException;

public class Test {

	public static void main(String[] args) throws InterruptedException, ExecutionException {
		
		RequestQueue rq = new RequestQueue();
		for(int i = 0; i < 10; i++)
			new ServerThread(rq, "ServerThread" + i).start();
		
		for(int i = 0; i < 10; i++)
			new ClientThread(rq,"ClientThread" + i).start();
		
	}
	
	
	
}


/**
 * 客户端请求
 * @author Administrator
 *
 */
class Request {
	private String name;
	
	public Request(String name) {
		this.name = name;
	}
	
	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	@Override
	public String toString() {
		return "Request [name=" + name + "]";
	}
	
}


class RequestQueue {
	@SuppressWarnings("rawtypes")
	private LinkedList queue = new LinkedList();
	public synchronized Request getRequest() {
		while(queue.size() == 0) {
			try {
				wait();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		return (Request)queue.removeFirst();
	}
	
	@SuppressWarnings("unchecked")
	public synchronized void addRequest(Request r) {
		queue.add(r);
		notifyAll();
	}
}

class ServerThread extends Thread{
	private RequestQueue rq;
	public ServerThread(RequestQueue rq,String name) {
		super(name);
		this.rq = rq;
	}
	
	public void run() {
		while(true) {
			final Request r = rq.getRequest();
			try {
				Thread.sleep(100);   //模拟处理请求的耗时
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			System.out.println(Thread.currentThread().getName() + " handles " + r);
		}
	}
}

class ClientThread extends Thread{
	private RequestQueue rq;
	public ClientThread(RequestQueue rq, String name) {
		super(name);
		this.rq = rq;
	}
	
	public void run() {
		for(int i = 0; i < 100; i++) {
			String name = "RequestID : " + i + " Thread_name: " + Thread.currentThread().getName();
			Request r = new Request(name);
			System.out.println(name);
			rq.addRequest(r);
			try {
				Thread.sleep(10);  //模拟客户端请求的速度，快于服务器处理的速度
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		System.out.println(Thread.currentThread().getName() + " End ");
	}
}



```
带有参数返回的请求
```
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.ExecutionException;

public class Test {

	public static void main(String[] args) throws InterruptedException, ExecutionException {
		
		RequestQueue rq = new RequestQueue();
		for(int i = 0; i < 10; i++)
			new ServerThread(rq, "ServerThread" + i).start();
		
		for(int i = 0; i < 10; i++)
			new ClientThread(rq,"ClientThread" + i).start();
		
	}
	
	
	
}


/**
 * 客户端请求
 * @author Administrator
 *
 */
class Request {
	private String name;
	Data data;   //返回数据
	
	public synchronized Data getData() {
		return data;
	}

	public synchronized void setData(Data data) {
		this.data = data;
	}

	public Request(String name) {
		this.name = name;
	}
	
	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	@Override
	public String toString() {
		return "Request [name=" + name + "]";
	}
	
}


class RequestQueue {
	@SuppressWarnings("rawtypes")
	private LinkedList queue = new LinkedList();
	public synchronized Request getRequest() {
		while(queue.size() == 0) {
			try {
				wait();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		return (Request)queue.removeFirst();
	}
	
	@SuppressWarnings("unchecked")
	public synchronized void addRequest(Request r) {
		queue.add(r);
		notifyAll();
	}
}

class ServerThread extends Thread{
	private RequestQueue rq;
	public ServerThread(RequestQueue rq,String name) {
		super(name);
		this.rq = rq;
	}
	
	public void run() {
		while(true) {
			final Request r = rq.getRequest();
			final FutureData fd = (FutureData)r.getData();
			try {
				Thread.sleep(100);   //模拟处理请求的耗时
				RealData rd = new RealData();  //RealData在4.1.1中实现
				fd.setResult(rd);              //这是结果
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			System.out.println(Thread.currentThread().getName() + " handles " + r);
		}
	}
}

class ClientThread extends Thread{
	private RequestQueue rq;
	private List<Request> myRequest = new ArrayList<Request>();
	public ClientThread(RequestQueue rq, String name) {
		super(name);
		this.rq = rq;
	}
	
	public void run() {
		for(int i = 0; i < 100; i++) {
			String name = "RequestID : " + i + " Thread_name: " + Thread.currentThread().getName();
			Request r = new Request(name);
			r.setData(new FutureData());  //设置返回值,FutureData具体实现在4.1.1中
			System.out.println(name);
			rq.addRequest(r);
			myRequest.add(r);
			try {
				Thread.sleep(10);  //模拟客户端请求的速度，快于服务器处理的速度
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		myRequest.stream().forEach(r -> {
			System.out.println(r.getData().getResult());
		});
		
		System.out.println(Thread.currentThread().getName() + " End ");
		
	}
}
```
#### 4.1.4.不变模式
* 核心思想:一个对象一旦创建,该对象的内部状态将不再改变，所以没有线程可以修改该对象的状态&数据, 同时内部状态也不会自行发生改变.基于这些特性,对不变对象的多线程操作不需要同步
* 使用场景:
  * 对象创建之后，每部状态和数据不在发生变化
  * 对象需要被共享,并被多线程频繁访问
* 实现不变模式需要注意如下几点:
  * 去除set方法，以及其他任意可以修改属性的方法
  * 将属性标记私有, 并用 final修饰
  * 没有子类可以修改它的行为, class用final修饰
  * 有一个可以创建完整对象的构造函数
实例代码:
```

final class Product{
	final String name;
	final int count;
	final double price;
	
	public Product(String name,int count,double price) {
		this.name = name;
		this.count = count;
		this.price = price;
	}

	public String getName() {
		return name;
	}

	public int getCount() {
		return count;
	}

	public double getPrice() {
		return price;
	}
	
	
}
```
#### 4.1.5.生产者消费者模式
* 核心思想：生产者和消费者线程利用缓冲器进行交互;生产者提交任务到缓冲器，消费者从缓冲区处理任务.
* 核心组件:共享内存缓冲区。作为消费者和生产者之间通讯的桥梁，避免了他们之间直接通讯。从而将生产者和消费者解耦.生产者和消费者不需要知道相互的存在。允许生产者和消费者在执行方面存在时间差，都可以通过缓冲区得到缓解。
* 简单的代码实现消费者-生产者
```
import java.util.Random;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;

public class Test {

	public static void main(String[] args) throws InterruptedException, ExecutionException {
		
		BlockingQueue<PCData> q = new LinkedBlockingQueue<>();
		Producer p1 = new Producer(q);
		Producer p2 = new Producer(q);
		Producer p3 = new Producer(q);
		Producer p4 = new Producer(q);
		Consumer c1 = new Consumer(q);
		Consumer c2 = new Consumer(q);
		
		ExecutorService es = Executors.newCachedThreadPool();
		es.execute(p1);
		es.execute(p2);
		es.execute(p3);
		es.execute(p4);
		es.execute(c1);
		es.execute(c2);
		
		Thread.sleep(10 * 1000);
		
		p1.stop();
		p2.stop();
		p3.stop();
		p4.stop();
		
		Thread.sleep(3000);
		
		es.shutdown();
		
	}
	
	
	
}

final class PCData{
	private final int intData;
	public PCData(int intData) {
		this.intData = intData;
	}
	public int getIntData() {
		return intData;
	}
	@Override
	public String toString() {
		return "PCData [intData=" + intData + "]";
	}
	
}

class Producer implements Runnable{

	private volatile boolean isRunning = true;
	private BlockingQueue<PCData> queue;
	private static final int SLEEPTIME = 1000;
	private static AtomicInteger count = new AtomicInteger();
	
	public Producer(BlockingQueue<PCData> q) {
		this.queue = q;
	}
	
	@Override
	public void run() {
		PCData d = null;
		System.out.println("Producer Thread" + Thread.currentThread().getId());
		Random r = new Random();
		while(isRunning) {
			try {
				Thread.sleep(r.nextInt(SLEEPTIME));
				if(!queue.offer(new PCData(count.incrementAndGet()))) {
					System.out.println("插入数据失败");
				}
				System.out.println("Producer Thread" + Thread.currentThread().getId() + " 生产数据");
			} catch (InterruptedException e) {
				e.printStackTrace();
				Thread.currentThread().interrupt();
			}
		}
		
	}
	
	public void stop() { isRunning = false; }
	
}

class Consumer implements Runnable{

	private BlockingQueue<PCData> queue;
	private static final int SLEEPTIME = 1000;
	
	public Consumer(BlockingQueue<PCData> q) {
		this.queue = q;
	}
	
	@Override
	public void run() {
		PCData d = null;
		System.out.println("Consumer Thread" + Thread.currentThread().getId());
		Random r = new Random();
		while(true) {
			try {
				PCData p = queue.take();
				if(null != p) {
					System.out.println("Consumer Thread" + Thread.currentThread().getId() + " ,消费数据 :" + p.getIntData());
					Thread.sleep(r.nextInt(SLEEPTIME));
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
				Thread.currentThread().interrupt();
			}
		}
		
	}
	
}
```
