## 4.并行程序开发及优化
### 4.1.并行程序设计模式
#### 4.1.1.Future模式







------


[4.1.1 Future 模式](#411Future模式)
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

