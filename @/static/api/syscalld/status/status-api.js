/**
 * syscalld 状态 API 客户端，用于嵌入HTML
 * 从 /api/syscalld/status 端点获取并显示系统状态信息
 */

function fetchSyscalldStatus() {
    // 从API获取状态信息
    fetch('/api/syscalld/status/?forceapi=true')
        .then(response => {
            if (!response.ok) {
                throw new Error('网络响应不正常');
            }
            return response.json();
        })
        .then(data => {
            // 在id为'status-display'的元素中显示状态信息
            const statusElement = document.getElementById('status-display');
            if (statusElement) {
                // 创建状态信息HTML
                let html = '<div class="status-container">';
                for (const [key, value] of Object.entries(data)) {
                    html += `<div class="status-item">
                        <span class="status-key">${key}:</span>
                        <span class="status-value">${value}</span>
                    </div>`;
                }
                html += '</div>';
                statusElement.innerHTML = html;
            }
        })
        .catch(error => {
            console.error('获取状态信息时出错:', error);
            const statusElement = document.getElementById('status-display');
            if (statusElement) {
                statusElement.innerHTML = '<div class="status-error">获取状态信息时出错</div>';
            }
        });
}

// 页面加载时获取状态
document.addEventListener('DOMContentLoaded', function() {
    fetchSyscalldStatus();
});

// 可选：添加手动刷新状态的按钮
function refreshStatus() {
    fetchSyscalldStatus();
}