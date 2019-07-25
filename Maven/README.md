# maven命令的使用方式
###### 清理
> mvn clean
###### 打包
> * mvn package
> * mvn package assembly:single
> * mvn package -Dmaven.test.skip=true
> * mvn clean compile
> * mvn clean install
> * mvn clean package
###### pom.xml中设置远程仓库
``` 
<repositories>
  <repository>
     <id>central</id>
     <url>http://maven.aliyun.com/nexus/content/groups/public/</url>
     <name>aliyun</name>
  </repository>
  <repository>
     <id>springsource-repos</id>
     <name>SpringSource Repository</name>
     <url>http://repo.spring.io/release/</url>
  </repository>
  <repository>
    <id>central-repos</id>
    <name>Central Repository</name>
    <url>http://repo.maven.apache.org/maven2</url>
  </repository>
  <repository>
    <id>central-repos2</id>
    <name>Central Repository 2</name>
    <url>http://repo1.maven.org/maven2/</url>
  </repository>
  </repositories>
  
<pluginRepositories>
  <pluginRepository>
    <id>central</id>
    <url>http://maven.aliyun.com/nexus/content/groups/public/</url>
    <name>aliyun</name>
  </pluginRepository>
</pluginRepositories>

```
