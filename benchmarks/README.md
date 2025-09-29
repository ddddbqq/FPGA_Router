本赛题所有测试用例设置的最大ratio约束为：512。

checker运行说明：
1. 进入到可执行文件路径下
2. ./checker <Testcase Dir> （举例说明: ./checker /home/public/testcase/sample01）
3. 执行完若输出错误信息，则说明结果不合法。若输出了max delay的值则说明结果合法。
4. Testcase Dir路径下要包含以下几个文件：design.net, design.fpga.out, design.info，design.topo，design.route.out。若有重新组网，则需要包含design.newtopo文件