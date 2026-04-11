/**
 * IP API 客户端，用于嵌入HTML
 * 从 /api/ip 端点获取并显示客户端IP信息
 */

function fetchClientIP() {
    // 从API获取IP信息
    fetch('/api/ip/?forceapi=true')
        .then(response => {
            if (!response.ok) {
                throw new Error('网络响应不正常');
            }
            return response.json();
        })
        .then(data => {
            // 在id为'ip-display'的元素中显示IP
            const ipElement = document.getElementById('ip-display');
            if (ipElement) {
                ipElement.textContent = data.ip || 'IP不可用';
            }
        })
        .catch(error => {
            console.error('获取IP时出错:', error);
            const ipElement = document.getElementById('ip-display');
            if (ipElement) {
                ipElement.textContent = '获取IP时出错';
            }
        });
}

// 页面加载时获取IP
document.addEventListener('DOMContentLoaded', function() {
    fetchClientIP();
});

// 可选：添加手动刷新IP的按钮
function refreshIP() {
    fetchClientIP();
}