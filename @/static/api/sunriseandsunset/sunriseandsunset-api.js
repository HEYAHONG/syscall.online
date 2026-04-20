/**
 * 日出日落 API 客户端
 * 从 /api/sunriseandsunset 端点获取日出日落时间
 */

// 预设城市坐标
const PRESET_CITIES = {
    chengdu:   { lat: 30.5728, lon: 104.0668, tz: 8 },
    beijing:   { lat: 39.9042, lon: 116.4074, tz: 8 },
    shanghai:  { lat: 31.2304, lon: 121.4737, tz: 8 },
    guangzhou: { lat: 23.1291, lon: 113.2644, tz: 8 },
    haerbin:   { lat: 45.8038, lon: 126.5350, tz: 8 },
};

/**
 * 设置预设城市
 * @param {string} city - 城市名称
 */
function setPreset(city) {
    const preset = PRESET_CITIES[city];
    if (preset) {
        document.getElementById('lat').value = preset.lat;
        document.getElementById('lon').value = preset.lon;
        document.getElementById('timezone').value = preset.tz;
    }
}

/**
 * 设置为今天的日期
 */
function useToday() {
    const today = new Date();
    const dateStr = today.toISOString().split('T')[0];
    document.getElementById('date').value = dateStr;
}

/**
 * 格式化时间为 HH:MM:SS
 * @param {object} time - 时间对象 {h, m, s}
 * @returns {string} 格式化后的时间字符串
 */
function formatTime(time) {
    if (!time) return '--:--:--';
    const h = String(time.h || 0).padStart(2, '0');
    const m = String(time.m || 0).padStart(2, '0');
    const s = String(time.s || 0).padStart(2, '0');
    return `${h}:${m}:${s}`;
}

/**
 * 显示错误信息
 * @param {string} message - 错误消息
 */
function showError(message) {
    const errorEl = document.getElementById('error');
    errorEl.textContent = message;
    errorEl.style.display = 'block';
    document.getElementById('result').classList.remove('show');
}

/**
 * 隐藏错误信息
 */
function hideError() {
    document.getElementById('error').style.display = 'none';
}

/**
 * 查询日出日落时间
 */
function querySunriseSunset() {
    // 获取输入值
    const lat = parseFloat(document.getElementById('lat').value);
    const lon = parseFloat(document.getElementById('lon').value);
    const timezone = parseInt(document.getElementById('timezone').value);
    const dateValue = document.getElementById('date').value;

    // 验证输入
    if (isNaN(lat) || lat < -90 || lat > 90) {
        showError('请输入有效的纬度 (-90 ~ 90)');
        return;
    }
    if (isNaN(lon) || lon < -180 || lon > 180) {
        showError('请输入有效的经度 (-180 ~ 180)');
        return;
    }
    if (isNaN(timezone) || timezone < -12 || timezone > 14) {
        showError('请输入有效的时区 (-12 ~ +14)');
        return;
    }

    // 解析日期
    let year, month, day;
    if (dateValue) {
        const parts = dateValue.split('-');
        year = parseInt(parts[0]);
        month = parseInt(parts[1]);
        day = parseInt(parts[2]);
    }

    // 构建查询参数
    const params = new URLSearchParams();
    params.append('lat', lat);
    params.append('lon', lon);
    params.append('timezone', timezone);
    if (year) params.append('year', year);
    if (month) params.append('month', month);
    if (day) params.append('day', day);
    params.append('forceapi', 'true');

    // 显示加载状态
    document.getElementById('sunrise-time').textContent = '加载中...';
    document.getElementById('sunset-time').textContent = '加载中...';
    document.getElementById('result').classList.add('show');
    hideError();

    // 发送请求
    fetch(`/api/sunriseandsunset/?${params.toString()}`)
        .then(response => {
            if (!response.ok) {
                throw new Error('网络响应不正常');
            }
            return response.json();
        })
        .then(data => {
            // 更新显示
            document.getElementById('sunrise-time').textContent = formatTime(data.sunrise);
            document.getElementById('sunset-time').textContent = formatTime(data.sunset);

            // 更新信息面板
            document.getElementById('info-lat').textContent = data.lat.toFixed(4) + '°' + (data.lat >= 0 ? 'N' : 'S');
            document.getElementById('info-lon').textContent = Math.abs(data.lon).toFixed(4) + '°' + (data.lon >= 0 ? 'E' : 'W');
            document.getElementById('info-date').textContent = `${data.year}-${String(data.month).padStart(2, '0')}-${String(data.day).padStart(2, '0')}`;
            document.getElementById('info-tz').textContent = `UTC${data.timezone >= 0 ? '+' : ''}${data.timezone}`;
        })
        .catch(error => {
            console.error('查询日出日落时间时出错:', error);
            showError('查询失败，请检查网络连接或稍后重试');
        });
}

// 页面加载时初始化
document.addEventListener('DOMContentLoaded', function() {
    // 设置今天的日期
    useToday();
});
