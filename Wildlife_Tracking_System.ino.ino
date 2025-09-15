import pygame
import random
import math
from datetime import datetime

# Initialize pygame
pygame.init()

# Screen dimensions
WIDTH, HEIGHT = 1000, 700
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("IoT-based Wildlife Tracking System Simulation")

# Colors
BACKGROUND = (230, 245, 255)
TEXT_COLOR = (30, 30, 30)
ANIMAL_COLORS = [(200, 50, 50), (50, 200, 50), (50, 50, 200), (200, 200, 50)]
SENSOR_COLOR = (100, 100, 200)
BASE_STATION_COLOR = (200, 100, 100)
RANGE_COLOR = (100, 200, 100, 50)

# Fonts
font_large = pygame.font.SysFont('Arial', 28)
font_medium = pygame.font.SysFont('Arial', 20)
font_small = pygame.font.SysFont('Arial', 16)

# Animal class
class Animal:
    def __init__(self, x, y, color, name):
        self.x = x
        self.y = y
        self.color = color
        self.name = name
        self.speed = random.uniform(0.5, 2.0)
        self.direction = random.uniform(0, 2 * math.pi)
        self.change_direction_counter = 0
        self.health = random.uniform(80, 100)
        self.temperature = random.uniform(36.5, 39.5)
        self.activity = random.uniform(50, 100)
        self.tracker_range = 150
        self.transmission_interval = 30
        self.transmission_counter = 0
        
    def move(self):
        # Change direction occasionally
        self.change_direction_counter -= 1
        if self.change_direction_counter <= 0:
            self.direction = random.uniform(0, 2 * math.pi)
            self.change_direction_counter = random.randint(30, 100)
            
        # Move in the current direction
        self.x += math.cos(self.direction) * self.speed
        self.y += math.sin(self.direction) * self.speed
        
        # Bounce off walls
        if self.x < 20 or self.x > WIDTH - 20:
            self.direction = math.pi - self.direction
        if self.y < 20 or self.y > HEIGHT - 20:
            self.direction = -self.direction
            
        # Keep within bounds
        self.x = max(20, min(WIDTH - 20, self.x))
        self.y = max(20, min(HEIGHT - 20, self.y))
        
        # Randomly change health metrics
        self.health = max(50, min(100, self.health + random.uniform(-0.1, 0.1)))
        self.temperature = max(35.0, min(41.0, self.temperature + random.uniform(-0.05, 0.05)))
        self.activity = max(0, min(100, self.activity + random.uniform(-1, 1)))
        
        # Count down to next transmission
        self.transmission_counter -= 1
        
    def draw(self, surface):
        # Draw animal
        pygame.draw.circle(surface, self.color, (int(self.x), int(self.y)), 15)
        
        # Draw tracker device
        pygame.draw.rect(surface, SENSOR_COLOR, (int(self.x) - 8, int(self.y) + 10, 16, 8))
        
        # Draw transmission range
        range_surface = pygame.Surface((self.tracker_range*2, self.tracker_range*2), pygame.SRCALPHA)
        pygame.draw.circle(range_surface, RANGE_COLOR, (self.tracker_range, self.tracker_range), self.tracker_range)
        surface.blit(range_surface, (int(self.x) - self.tracker_range, int(self.y) - self.tracker_range))
        
        # Draw animal name
        name_text = font_small.render(self.name, True, TEXT_COLOR)
        surface.blit(name_text, (int(self.x) - name_text.get_width() // 2, int(self.y) - 30))

# Base station class
class BaseStation:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.reception_range = 200
        self.received_data = []
        
    def draw(self, surface):
        # Draw base station
        pygame.draw.rect(surface, BASE_STATION_COLOR, (self.x - 20, self.y - 20, 40, 40))
        
        # Draw reception range
        range_surface = pygame.Surface((self.reception_range*2, self.reception_range*2), pygame.SRCALPHA)
        pygame.draw.circle(range_surface, (200, 100, 100, 50), (self.reception_range, self.reception_range), self.reception_range)
        surface.blit(range_surface, (int(self.x) - self.reception_range, int(self.y) - self.reception_range))
        
        # Draw base station label
        text = font_medium.render("Base Station", True, TEXT_COLOR)
        surface.blit(text, (self.x - text.get_width() // 2, self.y + 30))

# Create animals
animals = [
    Animal(random.randint(100, WIDTH-100), random.randint(100, HEIGHT-100), 
           ANIMAL_COLORS[0], "Elephant"),
    Animal(random.randint(100, WIDTH-100), random.randint(100, HEIGHT-100), 
           ANIMAL_COLORS[1], "Lion"),
    Animal(random.randint(100, WIDTH-100), random.randint(100, HEIGHT-100), 
           ANIMAL_COLORS[2], "Zebra"),
    Animal(random.randint(100, WIDTH-100), random.randint(100, HEIGHT-100), 
           ANIMAL_COLORS[3], "Giraffe")
]

# Create base station
base_station = BaseStation(WIDTH // 2, HEIGHT // 2)

# Main loop
clock = pygame.time.Clock()
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running = False
    
    # Clear screen
    screen.fill(BACKGROUND)
    
    # Draw title
    title_text = font_large.render("IoT-based Wildlife Tracking System Simulation", True, TEXT_COLOR)
    screen.blit(title_text, (WIDTH // 2 - title_text.get_width() // 2, 10))
    
    # Draw explanation
    explanation = [
        "This simulation shows how IoT devices can track wildlife movement and health parameters.",
        "Each animal has a tracker that transmits location, health, temperature, and activity data.",
        "The base station (red) receives data when animals are within range.",
        "Data is visualized on the right side of the screen."
    ]
    
    for i, line in enumerate(explanation):
        text = font_small.render(line, True, TEXT_COLOR)
        screen.blit(text, (WIDTH // 2 - text.get_width() // 2, 50 + i * 25))
    
    # Move and draw animals
    for animal in animals:
        animal.move()
        
        # Check if animal is in range of base station and ready to transmit
        distance = math.sqrt((animal.x - base_station.x)**2 + (animal.y - base_station.y)**2)
        if distance < base_station.reception_range and animal.transmission_counter <= 0:
            # Transmit data
            animal.transmission_counter = animal.transmission_interval
            base_station.received_data.append({
                'animal': animal.name,
                'time': datetime.now().strftime("%H:%M:%S"),
                'health': animal.health,
                'temperature': animal.temperature,
                'activity': animal.activity,
                'x': animal.x,
                'y': animal.y
            })
            # Keep only the last 10 readings
            if len(base_station.received_data) > 10:
                base_station.received_data.pop(0)
        
        animal.draw(screen)
    
    # Draw base station
    base_station.draw(screen)
    
    # Draw data panel
    pygame.draw.rect(screen, (240, 240, 240), (WIDTH - 320, 100, 300, HEIGHT - 120))
    pygame.draw.rect(screen, (200, 200, 200), (WIDTH - 320, 100, 300, 30))
    title = font_medium.render("Received Sensor Data", True, TEXT_COLOR)
    screen.blit(title, (WIDTH - 320 + 150 - title.get_width() // 2, 105))
    
    # Draw data
    if base_station.received_data:
        for i, data in enumerate(base_station.received_data):
            y_pos = 140 + i * 50
            if y_pos < HEIGHT - 50:
                text = font_small.render(f"{data['time']} - {data['animal']}", True, TEXT_COLOR)
                screen.blit(text, (WIDTH - 310, y_pos))
                
                text = font_small.render(f"Health: {data['health']:.1f}%", True, TEXT_COLOR)
                screen.blit(text, (WIDTH - 310, y_pos + 20))
                
                text = font_small.render(f"Temp: {data['temperature']:.1f}°C", True, TEXT_COLOR)
                screen.blit(text, (WIDTH - 180, y_pos + 20))
                
                text = font_small.render(f"Activity: {data['activity']:.1f}%", True, TEXT_COLOR)
                screen.blit(text, (WIDTH - 310, y_pos + 35))
    else:
        text = font_small.render("No data received yet...", True, TEXT_COLOR)
        screen.blit(text, (WIDTH - 310, 150))
    
    # Draw legend
    pygame.draw.rect(screen, (240, 240, 240), (10, HEIGHT - 120, 280, 110))
    pygame.draw.rect(screen, (200, 200, 200), (10, HEIGHT - 120, 280, 25))
    title = font_small.render("Legend", True, TEXT_COLOR)
    screen.blit(title, (150 - title.get_width() // 2, HEIGHT - 115))
    
    pygame.draw.circle(screen, ANIMAL_COLORS[0], (30, HEIGHT - 85), 10)
    text = font_small.render("Animal with tracker", True, TEXT_COLOR)
    screen.blit(text, (50, HEIGHT - 90))
    
    pygame.draw.rect(screen, BASE_STATION_COLOR, (25, HEIGHT - 60, 20, 20))
    text = font_small.render("Base station", True, TEXT_COLOR)
    screen.blit(text, (50, HEIGHT - 55))
    
    pygame.draw.circle(screen, RANGE_COLOR, (30, HEIGHT - 30), 10)
    text = font_small.render("Transmission/reception range", True, TEXT_COLOR)
    screen.blit(text, (50, HEIGHT - 35))
    
    # Update display
    pygame.display.flip()
    clock.tick(30)

pygame.quit()