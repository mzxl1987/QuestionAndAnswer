# SpringBoot启动过程

```java
@SpringBootApplication
public class SpringCloudWebApplication {

	public static void main(String[] args) {
		SpringApplication.run(SpringCloudWebApplication.class, args);
	}

}
```
> 启动过程主要分为2块
* 1.@SpringBootApplication注解的理解
* 2.SpringApplication.run();
## 1.@SpringBootApplication
该注解主要是做一些参数的配置,Package的扫描
> @SpringBootApplication包含了3个注解:
### * 1.1.[@SpringBootConfiguration]{https://github.com/mzxl1987/QuestionAndAnswer/blob/master/Java/srcCode/%40SpringBootConfiguration.java.md}
###   * 1.1.1.[@Configuration]{https://github.com/mzxl1987/QuestionAndAnswer/blob/master/Java/srcCode/%40Configuration.java.md}
      > 主要用于加载注解配置的类 & 包的扫描
### * 1.2.[@EnableAutoConfiguration]()
###   * 1.2.1.[@Import(AutoConfigurationImportSelector.class)]()
      > 其中调用到了[AutoConfigurationMetadataLoader ](https://github.com/mzxl1987/QuestionAndAnswer/blob/master/Java/srcCode/AutoConfigurationMetadataLoader.java.md)类
      ,该类中加载了spring-boot-autoconfigure-xxxxx.jar/META-INF/spring-autoconfigure-metadata.properities 配置文件,文件中配有自动加载的类属性
### * 1.3.[@ComponentScan]()   
      > 主要是对于包的扫描

