ip netns add net0
ip netns add net1
ip netns add net2
ip netns add net3
ip netns add net4
ip link add veth-0r type veth peer name veth-1l
ip link add veth-1r type veth peer name veth-2l
ip link add veth-2r type veth peer name veth-3l
ip link add veth-3r type veth peer name veth-4l
ip link set veth-0r netns net0
ip link set veth-1l netns net1
ip link set veth-1r netns net1
ip link set veth-2l netns net2
ip link set veth-2r netns net2
ip link set veth-3l netns net3
ip link set veth-3r netns net3
ip link set veth-4l netns net4
ip netns exec net0 ip addr add 192.168.1.2/24 dev veth-0r
ip netns exec net1 ip addr add 192.168.1.1/24 dev veth-1l
ip netns exec net1 ip addr add 192.168.3.1/24 dev veth-1r
# ip netns exec net2 ip addr add 192.168.3.2/24 dev veth-2l
# ip netns exec net2 ip addr add 192.168.4.1/24 dev veth-2r  # 不配置IP ?
ip netns exec net3 ip addr add 192.168.4.2/24 dev veth-3l
ip netns exec net3 ip addr add 192.168.5.2/24 dev veth-3r
ip netns exec net4 ip addr add 192.168.5.1/24 dev veth-4l
ip netns exec net0 ip link set veth-0r up
ip netns exec net1 ip link set veth-1l up
ip netns exec net1 ip link set veth-1r up
ip netns exec net2 ip link set veth-2l up
ip netns exec net2 ip link set veth-2r up
ip netns exec net3 ip link set veth-3l up
ip netns exec net3 ip link set veth-3r up
ip netns exec net4 ip link set veth-4l up
ip netns exec net0 ip r add default via 192.168.1.1
ip netns exec net4 ip r add default via 192.168.5.2  # 为两台PC配置网关
ip netns exec net0 ip link set lo up
ip netns exec net1 ip link set lo up
ip netns exec net2 ip link set lo up
ip netns exec net3 ip link set lo up
ip netns exec net4 ip link set lo up       # 开启回环设备

ip netns exec net0 ethtool -K veth-0r tx off
ip netns exec net1 ethtool -K veth-1l tx off
ip netns exec net1 ethtool -K veth-1r tx off
ip netns exec net2 ethtool -K veth-2l tx off
ip netns exec net2 ethtool -K veth-2r tx off
ip netns exec net3 ethtool -K veth-3l tx off
ip netns exec net3 ethtool -K veth-3r tx off
ip netns exec net4 ethtool -K veth-4l tx off  # 关闭checksum offload，这样抓包得到的checksum才正确
